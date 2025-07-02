/**
 * Custom Sidebar Link Resolver for Doxygen
 * Directly modifies href attributes for sidebar links that should point to external pages
 * Also expands all sidebar sections by default
 */

class CustomSidebarLinks {
    static init() {
        // Run immediately when DOM is ready, don't wait for window.load
        const runScript = () => {
            console.log("=== Custom Sidebar Links Script Starting ===");

            // Map of sidebar link text to target URLs
            const sidebarLinkMap = {
                "CoreVX Modules": "group__group__extensions.html",
                "CoreVX Functions": "group__group__all__kernels.html",
                "CoreVX APIs": "group__group__external.html",
                "CoreVX Files": "files.html",
                // Add more mappings here as needed:
            };

            let isUpdating = false; // Flag to prevent recursive calls

            // Function to restore highlighting state
            const restoreHighlighting = () => {
                const selectedLinkText = sessionStorage.getItem("selectedSidebarLink");
                if (selectedLinkText) {
                    const allSidebarLinks = sidebar.querySelectorAll("a");
                    allSidebarLinks.forEach(sidebarLink => {
                        if (sidebarLink.textContent.trim() === selectedLinkText) {
                            // Remove highlighting from all other links
                            allSidebarLinks.forEach(link => {
                                link.parentElement.classList.remove("selected");
                            });
                            // Add highlighting to the correct link
                            sidebarLink.parentElement.classList.add("selected");
                        }
                    });
                }
            };

            // Function to update sidebar links
            const updateSidebarLinks = () => {
                if (isUpdating) {
                    return;
                }

                isUpdating = true;

                let sidebar = document.getElementById("side-nav");
                if (!sidebar) {
                    sidebar = document.querySelector(".side-nav-resizable");
                }
                if (!sidebar) {
                    sidebar = document.querySelector("nav");
                }
                if (!sidebar) {
                    sidebar = document.querySelector("aside");
                }
                if (!sidebar) {
                    console.log("Sidebar not found.");
                    isUpdating = false;
                    return;
                }

                // Find all sidebar links that point to anchors or contain anchor fragments
                const links = sidebar.querySelectorAll("a[href*='#']");
                if (links.length === 0) {
                    isUpdating = false;
                    return; // No links to process
                }

                console.log("Found", links.length, "anchor links in sidebar");

                // Get all sidebar links to exclude them from searches
                const sidebarLinks = sidebar.querySelectorAll("a[href]");
                const sidebarHrefs = new Set();
                sidebarLinks.forEach(link => {
                    sidebarHrefs.add(link.getAttribute("href"));
                });

                // Get all links on the page
                const allPageLinks = Array.from(document.querySelectorAll("a[href]"));

                // Get all headings on the page
                const headings = document.querySelectorAll("h1, h2, h3, h4, h5, h6");

                for (let link of links) {
                    const linkText = link.textContent.trim();
                    const originalHref = link.getAttribute("href");

                    // Skip if already processed
                    if (link.classList.contains("custom-sidebar-link")) {
                        continue;
                    }

                    let targetUrl = null;

                    // Look for a heading with the exact same text that contains a link
                    for (let heading of headings) {
                        const headingText = heading.textContent.trim();

                        if (headingText === linkText) {
                            // Check if the heading itself is a link
                            let headingLink = null;
                            if (heading.tagName === 'A' && heading.hasAttribute('href')) {
                                headingLink = heading;
                            } else {
                                // Check for direct child links (not nested)
                                headingLink = heading.querySelector(":scope > a[href]");
                            }

                            if (headingLink) {
                                const href = headingLink.getAttribute("href");
                                if (href && !href.startsWith("#")) {
                                    targetUrl = href;
                                    break;
                                }
                            }
                        }
                    }

                    // If no heading match found, check if this link should point to a different page
                    if (!targetUrl) {
                        // Check if there are any links in the page that point to external pages with similar text
                        for (let pageLink of allPageLinks) {
                            const pageLinkHref = pageLink.getAttribute("href");
                            const pageLinkText = pageLink.textContent.trim();

                            // Skip if this link is already in the sidebar
                            if (sidebarHrefs.has(pageLinkHref)) {
                                continue;
                            }

                            // Check if text matches and link doesn't contain anchor fragments
                            if (pageLinkText === linkText && pageLinkHref && !pageLinkHref.includes("#")) {
                                targetUrl = pageLinkHref;
                                break;
                            }
                        }
                    }

                    // If still no target found, try to find a link with the same text that points to a different page
                    if (!targetUrl && originalHref.includes("#")) {
                        const basePage = originalHref.split("#")[0];

                        for (let pageLink of allPageLinks) {
                            const pageLinkHref = pageLink.getAttribute("href");
                            const pageLinkText = pageLink.textContent.trim();

                            // Skip if this link is already in the sidebar
                            if (sidebarHrefs.has(pageLinkHref)) {
                                continue;
                            }

                            // Check if text matches and link points to a different page
                            if (pageLinkText === linkText && pageLinkHref &&
                                pageLinkHref !== basePage && !pageLinkHref.includes("#")) {
                                targetUrl = pageLinkHref;
                                break;
                            }
                        }
                    }

                    // If still no match found, check the sidebar link map
                    if (!targetUrl) {
                        if (sidebarLinkMap[linkText]) {
                            const mappedUrl = sidebarLinkMap[linkText];

                            // First try to find this URL in the page links
                            for (let pageLink of allPageLinks) {
                                const pageLinkHref = pageLink.getAttribute("href");
                                if (pageLinkHref === mappedUrl) {
                                    targetUrl = pageLinkHref;
                                    break;
                                }
                            }

                            // If not found in page links, use the mapped URL directly
                            if (!targetUrl) {
                                console.log("Using mapped target for", linkText + ":", mappedUrl);
                                targetUrl = mappedUrl;
                            }
                        }
                    }

                    // If still no target found, try to find a link to the same base page without anchor
                    if (!targetUrl && originalHref.includes("#")) {
                        const basePage = originalHref.split("#")[0];

                        // Skip if this is a pure anchor link (no base page)
                        if (basePage) {
                            for (let pageLink of allPageLinks) {
                                const pageLinkHref = pageLink.getAttribute("href");

                                // Check if this link points to the same base page without anchor
                                if (pageLinkHref === basePage) {
                                    targetUrl = pageLinkHref;
                                    break;
                                }
                            }
                        }
                    }

                    if (targetUrl) {
                        // Simply update the href attribute
                        link.href = targetUrl;
                        link.classList.add("custom-sidebar-link");
                        console.log("Updated sidebar link:", linkText, "->", targetUrl);

                        // Add click handler to manage highlighting for external links
                        link.addEventListener("click", (e) => {
                            // Remove highlighting from all other links
                            const allSidebarLinks = sidebar.querySelectorAll("a");
                            allSidebarLinks.forEach(sidebarLink => {
                                sidebarLink.parentElement.classList.remove("selected");
                            });

                            // Add highlighting to the clicked link
                            link.parentElement.classList.add("selected");

                            // Store the selected state in sessionStorage for persistence
                            sessionStorage.setItem("selectedSidebarLink", linkText);
                        });
                    }
                }

                isUpdating = false; // Reset flag when done
            };

            // Set up MutationObserver to watch for changes to the sidebar
            const observer = new MutationObserver((mutations) => {
                let shouldUpdate = false;
                mutations.forEach((mutation) => {
                    if (mutation.type === 'childList') {
                        // Check if any links were added
                        mutation.addedNodes.forEach((node) => {
                            if (node.nodeType === Node.ELEMENT_NODE) {
                                if (node.matches && node.matches("a[href*='#']")) {
                                    shouldUpdate = true;
                                }
                                if (node.querySelectorAll) {
                                    const anchorLinks = node.querySelectorAll("a[href*='#']");
                                    if (anchorLinks.length > 0) {
                                        shouldUpdate = true;
                                    }
                                }
                            }
                        });
                    }
                });

                if (shouldUpdate) {
                    setTimeout(updateSidebarLinks, 50); // Small delay to ensure DOM is stable
                }
            });

            // Start observing the sidebar
            let sidebar = document.getElementById("side-nav");
            if (!sidebar) {
                sidebar = document.querySelector(".side-nav-resizable");
            }
            if (!sidebar) {
                sidebar = document.querySelector("nav");
            }
            if (!sidebar) {
                sidebar = document.querySelector("aside");
            }

            if (sidebar) {
                observer.observe(sidebar, {
                    childList: true,
                    subtree: true
                });

                // Also listen for click events on expand/collapse toggles
                sidebar.addEventListener("click", (e) => {
                    // Check if the click is on an expand/collapse toggle
                    const target = e.target;
                    if (target.classList.contains("arrow") ||
                        target.classList.contains("arrow-open") ||
                        target.classList.contains("arrow-closed") ||
                        target.closest(".arrow") ||
                        target.closest(".arrow-open") ||
                        target.closest(".arrow-closed")) {

                        setTimeout(updateSidebarLinks, 100); // Slightly longer delay for expand/collapse
                    }
                });
            }

            // Initial expansion and update
            updateSidebarLinks();
            restoreHighlighting();

            // Also run after a short delay to catch any late-loading content
            setTimeout(() => {
                updateSidebarLinks();
                restoreHighlighting();
            }, 500);

            // Add CSS for custom sidebar links
            const style = document.createElement('style');
            style.textContent = `
                .custom-sidebar-link {
                    color: #4ea1f7 !important;
                    font-weight: bold;
                }
                .custom-sidebar-link:hover {
                    text-decoration: underline;
                }
            `;
            document.head.appendChild(style);

            console.log("=== Custom Sidebar Links Script Finished ===");
        };

        // Run immediately if DOM is already ready
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', runScript);
        } else {
            runScript();
        }

        // Also run after window load to catch any content that loads after DOMContentLoaded
        window.addEventListener('load', () => {
            setTimeout(runScript, 100);
        });
    }
}