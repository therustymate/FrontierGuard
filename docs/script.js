(function () {
    const yearEl = document.getElementById("year");
    if (yearEl) {
        yearEl.textContent = String(new Date().getFullYear());
    }

    const navLinks = Array.from(document.querySelectorAll(".nav-link"));
    const sections = navLinks
        .map((a) => document.querySelector(a.getAttribute("href")))
        .filter(Boolean);

    const setActive = (id) => {
        navLinks.forEach((a) => {
            a.classList.toggle("is-active", a.getAttribute("href") === id);
        });
    };

    let isProgrammaticScroll = false;
    let scrollLockTimer = null;

    const lockObserver = (ms) => {
        isProgrammaticScroll = true;
        if (scrollLockTimer) {
            window.clearTimeout(scrollLockTimer);
        }
        scrollLockTimer = window.setTimeout(() => {
            isProgrammaticScroll = false;
        }, ms);
    };

    navLinks.forEach((link) => {
        link.addEventListener("click", (e) => {
            e.preventDefault();

            const id = link.getAttribute("href");
            const target = document.querySelector(id);
            if (!target) {
                return;
            }

            setActive(id);
            lockObserver(450);

            target.scrollIntoView({ behavior: "smooth", block: "start" });
        });
    });
    
    const visible = new Map();

    const pickBestSectionId = () => {
        let bestId = null;
        let bestTop = Infinity;

        visible.forEach((entry, id) => {
            const top = Math.abs(entry.boundingClientRect.top);
            if (top < bestTop) {
                bestTop = top;
                bestId = id;
            }
        });

        if (!bestId) {
            const y = window.scrollY + 10;
            for (const sec of sections) {
                const rect = sec.getBoundingClientRect();
                const absTop = window.scrollY + rect.top;
                if (absTop <= y) {
                    bestId = "#" + sec.id;
                }
            }
        }

        return bestId;
    };

    const observer = new IntersectionObserver(
        (entries) => {
            entries.forEach((entry) => {
                const id = "#" + entry.target.id;
                if (entry.isIntersecting) {
                    visible.set(id, entry);
                } else {
                    visible.delete(id);
                }
            });

            if (isProgrammaticScroll) {
                return;
            }

            const bestId = pickBestSectionId();
            if (bestId) {
                setActive(bestId);
            }
        },
        {
            root: null,
            rootMargin: "-35% 0px -55% 0px",
            threshold: [0, 0.15, 0.35, 0.6],
        }
    );

    sections.forEach((sec) => observer.observe(sec));

    const revealObserver = new IntersectionObserver(
        (entries) => {
            entries.forEach((entry) => {
                if (entry.isIntersecting) {
                    entry.target.classList.add("in");
                } else {
                    entry.target.classList.remove("in");
                }
            });
        },
        {
            threshold: 0.15,
        }
    );

    document.querySelectorAll(".reveal").forEach((el) => revealObserver.observe(el));

    window.addEventListener("load", () => {
        const hash = window.location.hash;
        if (hash) {
            setActive(hash);
        } else if (sections[0]) {
            setActive("#" + sections[0].id);
        }
    });
})();
