function doGet(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var lastRow = sheet.getLastRow();

  var temperature = e.parameter.temperature; // Replaced speed with temperature
  var visibility = e.parameter.visibility;
  var serialNumber = lastRow + 1;
  var timestamp = new Date();
  sheet.appendRow([serialNumber, timestamp, temperature, visibility]);
  return ContentService.createTextOutput(JSON.stringify({success: true}))
    .setMimeType(ContentService.MimeType.JSON);
}

function onOpen() {
  var ui = SpreadsheetApp.getUi();
  ui.createMenu('IoT Data')
    .addItem('Setup Sheet', 'setupSheet')
    .addToUi();
}

function setupSheet() {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  sheet.clear();
  sheet.appendRow(['S.No', 'Timestamp', 'Temperature (°C)', 'Visibility']); // Updated header for temperature
  
  var headerRange = sheet.getRange(1, 1, 1, 4);
  headerRange.setFontWeight('bold');
  headerRange.setBackground('#f3f3f3');
  sheet.setColumnWidth(1, 70);  // S.No
  sheet.setColumnWidth(2, 150); // Timestamp
  sheet.setColumnWidth(3, 130); // Temperature
  sheet.setColumnWidth(4, 100); // Visibility
  sheet.setFrozenRows(1);
  
  var visibilityRange = sheet.getRange('D2:D');
  var rule = SpreadsheetApp.newDataValidation()
    .requireValueInList(['Good', 'Moderate', 'Poor'], true)
    .build();
  visibilityRange.setDataValidation(rule);
}
