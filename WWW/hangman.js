const polishLetters = [...'AĄBCĆDEĘFGHIJKLŁMNŃOÓPQRSŚTUVWXYZŹŻ'];
const wordList = ['GAME OF THRONES','FIGHTCLUB','LORD OF THE RINGS','STAR WARS','FAST AND FURIOUS'];
let selectedWord = '';
let guessedLetters = [];
let mistakes = 0;
const maxMistakes = 8;

const wordDiv = document.getElementById('word');
const lettersDiv = document.getElementById('letters');
const messageDiv = document.getElementById('message');
const restartButton = document.getElementById('restartButton');
const canvas = document.getElementById('hangmanCanvas');
const ctx = canvas.getContext('2d');

// ---- Funkcje gry ----

function drawHangman() {
    ctx.lineWidth = 2;
    ctx.strokeStyle = '#FFFFFF';

    if (mistakes >= 1) { ctx.beginPath(); ctx.moveTo(10, 240); ctx.lineTo(190, 240); ctx.stroke(); }
    if (mistakes >= 2) { ctx.beginPath(); ctx.moveTo(50, 240); ctx.lineTo(50, 20); ctx.lineTo(120, 20); ctx.lineTo(120, 40); ctx.stroke(); }
    if (mistakes >= 3) { ctx.beginPath(); ctx.arc(120, 60, 20, 0, Math.PI * 2); ctx.stroke(); }
    if (mistakes >= 4) { ctx.beginPath(); ctx.moveTo(120, 80); ctx.lineTo(120, 150); ctx.stroke(); }
    if (mistakes >= 5) { ctx.beginPath(); ctx.moveTo(120, 100); ctx.lineTo(90, 130); ctx.stroke(); }
    if (mistakes >= 6) { ctx.beginPath(); ctx.moveTo(120, 100); ctx.lineTo(150, 130); ctx.stroke(); }
    if (mistakes >= 7) { ctx.beginPath(); ctx.moveTo(120, 150); ctx.lineTo(90, 200); ctx.stroke(); } // Lewa noga
    if (mistakes >= 8) { ctx.beginPath(); ctx.moveTo(120, 150); ctx.lineTo(150, 200); ctx.stroke(); } // Prawa noga
}

function displayWord() {
    wordDiv.innerHTML = selectedWord.split('').map(letter =>{
        if (!polishLetters.includes(letter)) {
            return '&nbsp;';
        }
        return guessedLetters.includes(letter) ? letter : '_'
    }).join(' ');
}

function generateLetters() {
    lettersDiv.innerHTML = '';
    polishLetters.forEach(letter => {
        const btn = document.createElement('button');
        btn.textContent = letter;
        btn.classList.add('letter');
        btn.addEventListener('click', () => guess(letter, btn));
        lettersDiv.appendChild(btn);
    });
}

function guess(letter, button) {
    button.disabled = true;
    button.classList.add('disabled');
    if (selectedWord.includes(letter)) {
        guessedLetters.push(letter);
        displayWord();
        checkWin();
    } else {
        mistakes++;
        drawHangman();
        checkLose();
    }
    saveGameState();
}

function checkWin() {
    if (selectedWord.split('').every(letter => guessedLetters.includes(letter))) {
        messageDiv.textContent = 'Gratulacje! Wygrałeś!';
        disableAllButtons();
        clearGameState();
    }
}

function checkLose() {
    if (mistakes >= maxMistakes) {
        messageDiv.textContent = `Przegrana! Słowo to: ${selectedWord}`;
        disableAllButtons();
        clearGameState();
    }
}

function disableAllButtons() {
    document.querySelectorAll('.letter').forEach(btn => {
        btn.disabled = true;
        btn.classList.add('disabled');
    });
}

function startGame() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    selectedWord = wordList[Math.floor(Math.random() * wordList.length)];
    guessedLetters = [' '];
    mistakes = 0;
    messageDiv.textContent = '';
    generateLetters();
    displayWord();
    saveGameState();
}

function restartGame() {
    clearGameState();
    startGame();
}

restartButton.addEventListener('click', restartGame);

// ---- Local Storage ----

function saveGameState() {
    const state = {
        selectedWord,
        guessedLetters,
        mistakes
    };
    localStorage.setItem('hangmanGameState', JSON.stringify(state));
}

function loadGameState() {
    const state = JSON.parse(localStorage.getItem('hangmanGameState'));
    if (state) {
        selectedWord = state.selectedWord;
        guessedLetters = state.guessedLetters;
        mistakes = state.mistakes;
        generateLetters();
        displayWord();
        for (const btn of document.querySelectorAll('.letter')) {
            if (guessedLetters.includes(btn.textContent) || !selectedWord.includes(btn.textContent)) {
                btn.disabled = true;
                btn.classList.add('disabled');
            }
        }
        for (let i = 0; i < mistakes; i++) {
            drawHangman();
        }
        checkWin();
        checkLose();
    } else {
        startGame();
    }
}

function clearGameState() {
    localStorage.removeItem('hangmanGameState');
}

// ---- Inicjalizacja ----
window.onload = loadGameState;
