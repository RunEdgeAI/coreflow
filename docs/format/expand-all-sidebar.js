console.log('[expand-all-sidebar] Script loaded');
(function () {
    function expandAllSidebarSectionsRecursive(maxTries, delayMs) {
        let tries = 0;
        function expandStep() {
            let arrows = Array.from(document.querySelectorAll('span.arrow'));
            let collapsedArrows = arrows.filter(arrow => arrow.textContent.trim() === '►');
            console.log(`[expand-all-sidebar] Iteration ${tries + 1}: Found ${collapsedArrows.length} collapsed arrows`);
            collapsedArrows.forEach((arrow, idx) => {
                console.log(`[expand-all-sidebar] Clicking collapsed arrow #${idx}`, arrow);
                arrow.click();
            });
            // Set all arrows to ▼ for consistency
            arrows.forEach((arrow, idx) => {
                if (arrow.textContent.trim() === '►' || arrow.textContent.trim() === '▼') {
                    arrow.textContent = '▼';
                }
            });
            tries++;
            if (collapsedArrows.length > 0 && tries < maxTries) {
                setTimeout(expandStep, delayMs);
            } else {
                console.log('[expand-all-sidebar] Expansion complete or max tries reached');
            }
        }
        expandStep();
    }

    function waitForSidebarAndExpand(maxWaitMs) {
        var waited = 0;
        var interval = setInterval(function () {
            var navTree = document.getElementById('nav-tree');
            var arrows = document.querySelectorAll('span.arrow');
            if (navTree && arrows.length > 0) {
                clearInterval(interval);
                console.log('[expand-all-sidebar] Sidebar detected, running recursive expansion logic');
                expandAllSidebarSectionsRecursive(10, 200);
            } else {
                waited += 100;
                if (waited >= maxWaitMs) {
                    clearInterval(interval);
                    console.log('[expand-all-sidebar] Timeout: Sidebar not detected after', maxWaitMs, 'ms');
                }
            }
        }, 100);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', function () {
            console.log('[expand-all-sidebar] DOMContentLoaded');
            waitForSidebarAndExpand(5000); // Wait up to 5 seconds
        });
    } else {
        console.log('[expand-all-sidebar] Document already loaded');
        waitForSidebarAndExpand(5000);
    }
})();