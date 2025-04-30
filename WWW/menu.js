document.addEventListener("DOMContentLoaded", function() {
    // Check if JS is enabled (hide noscript menu if it is)
    const noscriptMenu = document.querySelector(".noscript-menu");
    if (noscriptMenu) {
        noscriptMenu.style.display = "none";
    }

    // Hamburger menu functionality
    const hamburger = document.querySelector(".hamburger");
    const navLinks = document.querySelector(".nav-links:not(.noscript-menu)");

    if (hamburger && navLinks) {
        hamburger.addEventListener("click", () => {
            navLinks.classList.toggle("active");
            // Optional: change hamburger icon when menu is open
            hamburger.textContent = navLinks.classList.contains("active") ? "✕" : "☰";
        });
    }
});