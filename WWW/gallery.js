document.addEventListener('DOMContentLoaded', function() {
    // Image URLs array
    const imageUrls = [
        { url: "https://i.pinimg.com/736x/89/b1/93/89b193af5e69a308d02c4b797483504e.jpg", alt: "https://pin.it/38Qkku0dd" },
        { url: "https://i.pinimg.com/736x/10/ca/04/10ca04899df806effc417ebeeb919ac9.jpg", alt: "https://pin.it/7wk9jvR2B" },
        { url: "https://i.pinimg.com/736x/72/6e/4b/726e4b449dea46a63af23cce6dbf7dad.jpg", alt: "https://pin.it/4jhOdJZ6o" },
        { url: "https://i.pinimg.com/736x/f4/62/d7/f462d7db139e21fb4285befee09b7f88.jpg", alt: "https://pin.it/1n74c4hFe" },
        { url: "https://i.pinimg.com/736x/bd/a0/7c/bda07c56b246c5e6030277ac6655994b.jpg", alt: "https://pin.it/y6PziPgkt" },
        { url: "https://i.pinimg.com/736x/c6/40/21/c64021026856c78419585f0dfac96732.jpg", alt: "https://pin.it/7zu5N81q9" },
        { url: "https://i.pinimg.com/736x/20/11/7e/20117ea0669def1b475890229ec3cdaf.jpg", alt: "https://pin.it/39FNPQ6C0" },
        { url: "https://i.pinimg.com/736x/7f/2d/0b/7f2d0b24ea645545c41359c160d6b16c.jpg", alt: "https://pin.it/7298dPRfa" },
        { url: "https://i.pinimg.com/736x/3e/08/f3/3e08f35b13f5290edee5dfc221c9f025.jpg", alt: "https://pin.it/64kzPxwPT" }
    ];

    // Function to load a single image
    function loadImage(imageData) {
        return new Promise((resolve, reject) => {
            const img = new Image();
            img.src = imageData.url;
            img.alt = imageData.alt;
            img.onload = () => resolve(img);
            img.onerror = () => reject(new Error(`Failed to load image: ${imageData.url}`));
        });
    }

    // Load all images in parallel
    function loadAllImages() {
        const galleryContainer = document.getElementById('gallery-container');
        
        // Clear loading message
        galleryContainer.innerHTML = '';
        
        // Create promises for all images
        const imagePromises = imageUrls.map(imageData => 
            loadImage(imageData)
                .then(img => {
                    const article = document.createElement('article');
                    article.className = 'galleryItem';
                    article.appendChild(img);
                    return article;
                })
                .catch(error => {
                    console.error(error);
                    const errorElement = document.createElement('div');
                    errorElement.className = 'error';
                    errorElement.textContent = 'Image failed to load';
                    return errorElement;
                })
        );

        // When all promises resolve, add images to gallery
        Promise.all(imagePromises)
            .then(articles => {
                articles.forEach(article => {
                    galleryContainer.appendChild(article);
                });
            })
            .catch(error => {
                console.error('Error loading images:', error);
                galleryContainer.innerHTML = '<div class="error">Failed to load gallery. Please try again later.</div>';
            });
    }

    // Initialize gallery
    loadAllImages();
});