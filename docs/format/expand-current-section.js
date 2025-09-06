console.log('[expand-current-section] Script loaded');
(function () {
    function expandCurrentSectionPath() {
        // Get the current page URL or title to identify the current section
        const currentPath = window.location.pathname;
        const currentTitle = document.title;
        const currentHref = window.location.href;

        console.log('[expand-current-section] Current path:', currentPath);
        console.log('[expand-current-section] Current title:', currentTitle);
        console.log('[expand-current-section] Current href:', currentHref);

        // Find all navigation links in the sidebar
        const navLinks = Array.from(document.querySelectorAll('#nav-tree a'));
        console.log('[expand-current-section] Found', navLinks.length, 'navigation links');

        let currentLink = null;

        // Try multiple strategies to find the current page link
        for (const link of navLinks) {
            const href = link.getAttribute('href');
            const text = link.textContent.trim();

            console.log('[expand-current-section] Checking link:', { href, text });

            // Strategy 1: Exact href match (most reliable)
            if (href && href !== 'javascript:void(0)' && currentHref.includes(href)) {
                currentLink = link;
                console.log('[expand-current-section] Found current page link by exact href match:', href);
                break;
            }

            // Strategy 2: Filename match (ignoring path and extension)
            if (href && href !== 'javascript:void(0)') {
                const hrefFilename = href.split('/').pop().split('.')[0];
                const currentFilename = currentPath.split('/').pop().split('.')[0];
                if (hrefFilename && currentFilename && hrefFilename === currentFilename) {
                    currentLink = link;
                    console.log('[expand-current-section] Found current page link by filename match:', hrefFilename);
                    break;
                }
            }

            // Strategy 3: Check if link is currently active/selected (has specific classes or attributes)
            if (link.classList.contains('current') || link.classList.contains('active') ||
                link.getAttribute('aria-current') === 'page' || link.getAttribute('aria-current') === 'true') {
                currentLink = link;
                console.log('[expand-current-section] Found current page link by active state:', href);
                break;
            }

            // Strategy 4: Text content match (only as last resort and more specific)
            if (text && currentTitle.includes(text) && text.length > 3) {
                // Only match if the text is substantial and appears to be a page title
                // Avoid matching generic navigation terms
                const genericTerms = ['CoreFlow', 'Home', 'Index', 'Main', 'Overview'];
                if (!genericTerms.includes(text)) {
                    currentLink = link;
                    console.log('[expand-current-section] Found current page link by title match:', text);
                    break;
                }
            }
        }

        if (!currentLink) {
            console.log('[expand-current-section] Could not find current page in sidebar. Available links:');
            navLinks.forEach((link, index) => {
                console.log(`  ${index}: href="${link.getAttribute('href')}" text="${link.textContent.trim()}"`);
            });

            // Fallback: Try to find a parent section that might contain the current page
            console.log('[expand-current-section] Attempting fallback: looking for parent sections...');

            // Look for sections that might be related to the current page
            const currentFilename = currentPath.split('/').pop().split('.')[0];
            const possibleMatches = navLinks.filter(link => {
                const href = link.getAttribute('href');
                const text = link.textContent.trim().toLowerCase();
                return href && href !== 'javascript:void(0)' &&
                       (text.includes('example') || text.includes('sample') ||
                        text.includes('demo') || text.includes('tutorial'));
            });

            if (possibleMatches.length > 0) {
                currentLink = possibleMatches[0];
                console.log('[expand-current-section] Using fallback match:', currentLink);
            } else {
                console.log('[expand-current-section] No suitable fallback found');

                // For home/index pages, try to expand just the main sections, not everything
                if (currentPath.includes('index.html') || currentPath.endsWith('/') || currentPath === '' || currentTitle.toLowerCase().includes('home')) {
                    console.log('[expand-current-section] Detected home page, expanding main sections only');
                    expandMainSectionsOnly();
                } else {
                    console.log('[expand-current-section] Expanding all sections as fallback');
                    expandAllSections();
                }
                return;
            }
        }

        console.log('[expand-current-section] Found current link:', currentLink);

        // Find the path to the current link by traversing up the DOM tree
        const pathToCurrent = [];
        let element = currentLink;

        while (element && element !== document.getElementById('nav-tree')) {
            if (element.tagName === 'LI') {
                pathToCurrent.unshift(element);
            }
            element = element.parentElement;
        }

        console.log('[expand-current-section] Path to current section:', pathToCurrent.length, 'elements');

        // Expand only the sections in the path to the current page
        let expandedCount = 0;
        pathToCurrent.forEach((liElement, index) => {
            const arrow = liElement.querySelector('span.arrow');
            if (arrow) {
                const arrowText = arrow.textContent.trim();
                console.log(`[expand-current-section] Section ${index + 1}/${pathToCurrent.length}: arrow="${arrowText}"`);

                if (arrowText === '►') {
                    console.log(`[expand-current-section] Expanding section ${index + 1}/${pathToCurrent.length}`);
                    arrow.click();
                    // Set the arrow to expanded state
                    arrow.textContent = '▼';
                    expandedCount++;
                }
            } else {
                console.log(`[expand-current-section] Section ${index + 1}/${pathToCurrent.length}: No arrow found`);
            }
        });

        console.log(`[expand-current-section] Expanded ${expandedCount} sections`);

        // Now expand subsections within the current section's path
        // But be more selective for home/index pages
        if (currentPath.includes('index.html') || currentPath.endsWith('/') || currentPath === '' || currentTitle.toLowerCase().includes('home')) {
            console.log('[expand-current-section] Home page detected, skipping subsection expansion for cleaner view');
            // Don't expand any subsections on home page - keep it clean
        } else {
            expandSubsectionsInPath(pathToCurrent);
        }
    }

    function expandSubsectionsInPath(pathElements) {
        console.log('[expand-current-section] Expanding subsections within current section path...');

        // Only expand subsections in the last (deepest) element of the path
        // This represents the current section itself, not its parent sections
        if (pathElements.length > 0) {
            const currentSectionElement = pathElements[pathElements.length - 1];
            console.log('[expand-current-section] Focusing on current section element for subsections');

            // Find collapsed arrows only within the current section element
            const collapsedArrows = currentSectionElement.querySelectorAll('span.arrow');
            let expandedCount = 0;

            collapsedArrows.forEach((arrow, index) => {
                if (arrow.textContent.trim() === '►') {
                    console.log(`[expand-current-section] Expanding subsection ${index + 1} in current section`);
                    arrow.click();
                    // Set the arrow to expanded state
                    arrow.textContent = '▼';
                    expandedCount++;
                }
            });

            console.log(`[expand-current-section] Expanded ${expandedCount} subsections in current section`);
        } else {
            console.log('[expand-current-section] No path elements found, skipping subsection expansion');
        }

        console.log('[expand-current-section] Subsections expansion complete');
    }

    function expandFirstLevelSubsectionsOnly(pathElements) {
        console.log('[expand-current-section] Expanding only first level subsections for home page...');

        // Only expand subsections in the last (deepest) element of the path
        if (pathElements.length > 0) {
            const currentSectionElement = pathElements[pathElements.length - 1];
            console.log('[expand-current-section] Focusing on current section element for first level subsections');

            // Find only direct child arrows (first level subsections)
            const directChildren = Array.from(currentSectionElement.children).filter(child => child.tagName === 'UL' || child.tagName === 'OL');
            let expandedCount = 0;

            directChildren.forEach((list, listIndex) => {
                const firstLevelArrows = Array.from(list.children)
                    .filter(li => li.tagName === 'LI')
                    .map(li => li.querySelector('span.arrow'))
                    .filter(arrow => arrow && arrow.textContent.trim() === '►');

                firstLevelArrows.forEach((arrow, index) => {
                    console.log(`[expand-current-section] Expanding first level subsection ${index + 1} in list ${listIndex + 1}`);
                    arrow.click();
                    arrow.textContent = '▼';
                    expandedCount++;
                });
            });

            console.log(`[expand-current-section] Expanded ${expandedCount} first level subsections`);
        } else {
            console.log('[expand-current-section] No path elements found, skipping first level subsection expansion');
        }

        console.log('[expand-current-section] First level subsections expansion complete');
    }

    function expandAllSections() {
        console.log('[expand-current-section] Fallback: Expanding all sections');
        let arrows = Array.from(document.querySelectorAll('span.arrow'));
        let collapsedArrows = arrows.filter(arrow => arrow.textContent.trim() === '►');

        console.log(`[expand-current-section] Found ${collapsedArrows.length} collapsed arrows to expand`);

        collapsedArrows.forEach((arrow, idx) => {
            console.log(`[expand-current-section] Clicking collapsed arrow #${idx}`);
            arrow.click();
        });

        // Set all arrows to ▼ for consistency
        arrows.forEach((arrow, idx) => {
            if (arrow.textContent.trim() === '►' || arrow.textContent.trim() === '▼') {
                arrow.textContent = '▼';
            }
        });

        console.log('[expand-current-section] All sections expanded');
    }

    function expandMainSectionsOnly() {
        console.log('[expand-current-section] Expanding main sections only for home page');

        // Find all top-level arrows (direct children of nav-tree)
        const navTree = document.getElementById('nav-tree');
        if (!navTree) {
            console.log('[expand-current-section] Nav tree not found');
            return;
        }

        // Get direct child list items of nav-tree
        const mainSections = Array.from(navTree.children).filter(child => child.tagName === 'LI');
        let expandedCount = 0;

        mainSections.forEach((section, index) => {
            const arrow = section.querySelector('span.arrow');
            if (arrow && arrow.textContent.trim() === '►') {
                console.log(`[expand-current-section] Expanding main section ${index + 1}`);
                arrow.click();
                arrow.textContent = '▼';
                expandedCount++;
            }
        });

        console.log(`[expand-current-section] Expanded ${expandedCount} main sections`);
    }

    function waitForSidebarAndExpandCurrent(maxWaitMs) {
        var waited = 0;
        var interval = setInterval(function () {
            var navTree = document.getElementById('nav-tree');
            var arrows = document.querySelectorAll('span.arrow');
            if (navTree && arrows.length > 0) {
                clearInterval(interval);
                console.log('[expand-current-section] Sidebar detected, expanding current section path');
                // Add a small delay to ensure the sidebar is fully rendered
                setTimeout(expandCurrentSectionPath, 100);
            } else {
                waited += 100;
                if (waited >= maxWaitMs) {
                    clearInterval(interval);
                    console.log('[expand-current-section] Timeout: Sidebar not detected after', maxWaitMs, 'ms');
                }
            }
        }, 100);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', function () {
            console.log('[expand-current-section] DOMContentLoaded');
            waitForSidebarAndExpandCurrent(5000); // Wait up to 5 seconds
        });
    } else {
        console.log('[expand-current-section] Document already loaded');
        waitForSidebarAndExpandCurrent(5000);
    }
})();

