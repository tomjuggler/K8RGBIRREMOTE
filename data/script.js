// Tab switching
document.querySelectorAll('.tab-button').forEach(button => {
    button.addEventListener('click', () => {
        document.querySelectorAll('.tab-button').forEach(btn => btn.classList.remove('active'));
        document.querySelectorAll('.tab-content').forEach(content => content.classList.remove('active'));
        button.classList.add('active');
        document.getElementById(button.dataset.tab).classList.add('active');
    });
});

// Button click handling for both grids
document.getElementById('button-grid-k8').addEventListener('click', handleButtonClick);
document.getElementById('button-grid-chinese').addEventListener('click', handleButtonClick);

function handleButtonClick(event) {
    if (event.target.tagName === 'BUTTON') {
        const action = event.target.dataset.action;
        fetch(`/action?do=${action}`)
            .then(response => {
                if (!response.ok) {
                    console.error('Error sending command');
                }
            })
            .catch(error => console.error('Fetch error:', error));
    }
}
\n// Speed slider with debounce
let speedSliderTimeout = null;
const speedSlider = document.getElementById('speed-slider');
const speedValue = document.getElementById('speed-value');

speedSlider.addEventListener('input', function() {
    speedValue.textContent = this.value;

    // Clear any existing timeout
    if (speedSliderTimeout) {
        clearTimeout(speedSliderTimeout);
    }

    // Set new timeout to update after slider stops
    speedSliderTimeout = setTimeout(() => {
        updateSpeed(this.value);
    }, 300); // Update 300ms after last change
});

function updateSpeed(speedMs) {
    fetch(`/set_speed?speed=${speedMs}`)
        .then(response => {
            if (!response.ok) {
                console.error('Error setting speed');
            } else {
                console.log(`Speed set to ${speedMs}ms`);
            }
        })
        .catch(error => console.error('Fetch error:', error));
}
