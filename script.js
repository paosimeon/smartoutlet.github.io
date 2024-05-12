function toggleMenu() {
  const menu = document.querySelector(".menu-links");
  const icon = document.querySelector(".hamburger-icon");
  menu.classList.toggle("open");
  icon.classList.toggle("open");
}
document.getElementById('resetButton').addEventListener('click', resetData);
// Function to reset chart data and voltage fluctuation table
function resetData() {
  try {
      // Reset chart data
      energyReadings = [];
      timeLabels = [];
      localStorage.setItem('energyReadings', JSON.stringify(energyReadings));
      localStorage.setItem('timeLabels', JSON.stringify(timeLabels));
      energyChart.data.labels = [];
      energyChart.data.datasets[0].data = [];
      energyChart.update();

      // Reset voltage fluctuation table
      var table = document.getElementById('voltageFluctuationTable');
      table.innerHTML = '<tr><th>Date</th><th>Time</th><th>Voltage (V)</th></tr>';

      // Clear voltage fluctuation data from localStorage
      localStorage.removeItem('voltageFluctuations');
  } catch (error) {
      console.error('Error resetting data:', error);
  }
}