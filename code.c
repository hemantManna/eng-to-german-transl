#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi credentials
const char* ssid = "kakashi";
const char* password = "halwa123";

// Web server on port 80
WebServer server(80);

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions
const int BUTTON_PIN = 4;
const int RED_LED = 16;
const int YELLOW_LED = 17;
const int GREEN_LED = 18;

// Application state variables
bool isTranslating = false;
String currentEnglishText = "";
String currentGermanText = "";
unsigned long processingStartTime = 0;
const unsigned long PROCESSING_TIME = 4000; // 4 seconds processing time
int lastProgress = -1;

// Function prototypes
void handleButton();
void handleTranslationState();
void handleRoot();
void handleTranslate();
void handleProgress();
void handleStatus();
String translateSentence(String englishSentence);
String searchTranslation(String englishWord);
void resetSystem();
void allLEDsOff();

// Expanded English to German dictionary
const int DICTIONARY_SIZE = 215;

struct DictionaryEntry {
  String english;
  String german;
};

DictionaryEntry dictionary[DICTIONARY_SIZE] = {
  {"hello", "hallo"},
  {"goodbye", "auf wiedersehen"},
  {"thank you", "danke"},
  {"thankyou", "danke"},
  {"please", "bitte"},
  {"yes", "ja"},
  {"no", "nein"},
  {"sorry", "entschuldigung"},
  {"excuse me", "entschuldigung"},
  {"good morning", "guten morgen"},
  {"good night", "gute nacht"},
  {"good evening", "guten abend"},
  {"welcome", "willkommen"},
  
  // Questions & Basic Words
  {"what", "was"},
  {"when", "wann"},
  {"where", "wo"},
  {"why", "warum"},
  {"how", "wie"},
  {"who", "wer"},
  {"which", "welche"},
  {"this", "dies"},
  {"that", "das"},
  {"here", "hier"},
  {"there", "dort"},
  
  // Time & Calendar
  {"time", "zeit"},
  {"day", "tag"},
  {"night", "nacht"},
  {"week", "woche"},
  {"month", "monat"},
  {"year", "jahr"},
  {"today", "heute"},
  {"tomorrow", "morgen"},
  {"yesterday", "gestern"},
  {"now", "jetzt"},
  {"later", "später"},
  {"soon", "bald"},
  {"always", "immer"},
  {"never", "nie"},
  {"sometimes", "manchmal"},
  {"monday", "Montag"},
  {"tuesday", "Dienstag"},
  {"wednesday", "Mittwoch"},
  {"thursday", "Donnerstag"},
  {"friday", "Freitag"},
  {"saturday", "Samstag"},
  {"sunday", "Sonntag"},
  
  // People & Relationships
  {"friend", "freund"},
  {"family", "familie"},
  {"mother", "mutter"},
  {"father", "vater"},
  {"son", "sohn"},
  {"daughter", "tochter"},
  {"brother", "bruder"},
  {"sister", "schwester"},
  {"man", "mann"},
  {"woman", "frau"},
  {"child", "kind"},
  {"people", "menschen"},
  
  // Food & Drinks
  {"water", "wasser"},
  {"food", "essen"},
  {"bread", "brot"},
  {"milk", "milch"},
  {"coffee", "kaffee"},
  {"tea", "tee"},
  {"beer", "bier"},
  {"wine", "wein"},
  {"apple", "apfel"},
  {"banana", "banane"},
  {"meat", "fleisch"},
  {"fish", "fisch"},
  {"egg", "ei"},
  {"cheese", "käse"},
  {"sugar", "zucker"},
  
  // Home & Objects
  {"house", "haus"},
  {"home", "zuhause"},
  {"room", "zimmer"},
  {"door", "tür"},
  {"window", "fenster"},
  {"table", "tisch"},
  {"chair", "stuhl"},
  {"bed", "bett"},
  {"kitchen", "küche"},
  {"bathroom", "badezimmer"},
  {"garden", "garten"},
  
  // Transportation
  {"car", "auto"},
  {"bus", "bus"},
  {"train", "zug"},
  {"bicycle", "fahrrad"},
  {"street", "straße"},
  {"road", "straße"},
  {"airport", "flughafen"},
  {"station", "bahnhof"},
  
  // City & Places
  {"city", "stadt"},
  {"country", "land"},
  {"world", "welt"},
  {"shop", "geschäft"},
  {"market", "markt"},
  {"restaurant", "restaurant"},
  {"hotel", "hotel"},
  {"school", "schule"},
  {"university", "universität"},
  {"hospital", "krankenhaus"},
  {"bank", "bank"},
  {"park", "park"},
  
  // Technology & Objects
  {"computer", "computer"},
  {"phone", "telefon"},
  {"internet", "internet"},
  {"book", "buch"},
  {"pen", "stift"},
  {"paper", "papier"},
  {"money", "geld"},
  {"key", "schlüssel"},
  {"clock", "uhr"},
  {"light", "licht"},
  {"bag", "tasche"},
  {"clothes", "kleidung"},
  
  // Work & Education
  {"work", "arbeit"},
  {"job", "job"},
  {"student", "student"},
  {"teacher", "lehrer"},
  {"doctor", "arzt"},
  {"engineer", "ingenieur"},
  {"study", "studieren"},
  {"learn", "lernen"},
  {"read", "lesen"},
  {"write", "schreiben"},
  
  // Emotions & Feelings
  {"love", "liebe"},
  {"happy", "glücklich"},
  {"sad", "traurig"},
  {"angry", "wütend"},
  {"tired", "müde"},
  {"hungry", "hungrig"},
  {"thirsty", "durstig"},
  
  // Colors
  {"red", "rot"},
  {"blue", "blau"},
  {"green", "grün"},
  {"yellow", "gelb"},
  {"black", "schwarz"},
  {"white", "weiß"},
  
  // Numbers
  {"one", "eins"},
  {"two", "zwei"},
  {"three", "drei"},
  {"four", "vier"},
  {"five", "fünf"},
  {"six", "sechs"},
  {"seven", "sieben"},
  {"eight", "acht"},
  {"nine", "neun"},
  {"ten", "zehn"},
  
  // Weather & Nature
  {"sun", "sonne"},
  {"moon", "mond"},
  {"star", "stern"},
  {"sky", "himmel"},
  {"rain", "regen"},
  {"snow", "schnee"},
  {"wind", "wind"},
  {"tree", "baum"},
  {"flower", "blume"},
  {"animal", "tier"},
  {"dog", "hund"},
  {"cat", "katze"},
  
  // Verbs & Actions
  {"go", "gehen"},
  {"come", "kommen"},
  {"see", "sehen"},
  {"eat", "essen"},
  {"drink", "trinken"},
  {"sleep", "schlafen"},
  {"help", "hilfe"},
  {"find", "finden"},
  {"buy", "kaufen"},
  {"sell", "verkaufen"},
  {"open", "öffnen"},
  {"close", "schließen"},
  {"start", "starten"},
  {"stop", "stoppen"},
  {"wait", "warten"},
  
  // Additional words for better sentence translation
  {"is", "ist"},
  {"i", "ich"},
  {"have", "haben"},
  {"a", "a"},
  {"are", "sind"},
  {"am", "bin"},
  {"the", "das"},
  {"a", "eine"},
  {"an", "ein/eine"},
  {"and", "und"},
  {"or", "oder"},
  {"but", "aber"},
  {"with", "mit"},
  {"without", "ohne"},
  {"in", "in"},
  {"on", "auf"},
  {"at", "an"},
  {"to", "zu"},
  {"from", "von"},
  {"for", "für"},
  {"of", "von"},
  {"by", "durch"},
  {"as", "als"},
  {"it", "es"},
  {"he", "er"},
  {"she", "sie"},
  {"we", "wir"},
  {"they", "sie"},
  {"my", "mein"},
  {"your", "dein"},
  {"his", "sein"},
  {"her", "ihr"},
  {"our", "unser"},
  {"their", "ihr"},
  {"me", "mich"},
  {"you", "dich"},
  {"him", "ihn"},
  {"us", "uns"},
  {"them", "sie"}
};

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  // Turn off all LEDs initially
  allLEDsOff();
  
  // Show startup message on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("WiFi Translator");
  display.println("Starting...");
  display.display();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Update OLED with IP address
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("WiFi Connected!");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.println("Web server ready");
    display.display();
    
    // Setup web server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/translate", HTTP_POST, handleTranslate);
    server.on("/progress", HTTP_GET, handleProgress);
    server.on("/status", HTTP_GET, handleStatus);
    
    // Start web server
    server.begin();
    Serial.println("HTTP server started");
    
    // Blink Green LED to indicate ready state
    for(int i = 0; i < 3; i++) {
      digitalWrite(GREEN_LED, HIGH);
      delay(200);
      digitalWrite(GREEN_LED, LOW);
      delay(200);
    }
    
  } else {
    Serial.println("\nFailed to connect to WiFi!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("WiFi Failed!");
    display.println("Check credentials");
    display.display();
    
    // Blink Red LED to indicate error
    while(1) {
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      delay(500);
    }
  }
}

void loop() {
  // Handle web server clients
  server.handleClient();
  
  // Handle button press for manual reset
  handleButton();
  
  // Handle translation state machine
  handleTranslationState();
}

void handleButton() {
  static int lastButtonState = HIGH;
  int currentButtonState = digitalRead(BUTTON_PIN);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      // Button pressed - reset system
      resetSystem();
    }
  }
  lastButtonState = currentButtonState;
}

void handleTranslationState() {
  if (isTranslating) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - processingStartTime;
    
    if (elapsedTime <= PROCESSING_TIME) {
      // Still processing - update progress on OLED
      int progress = (elapsedTime * 100) / PROCESSING_TIME;
      
      // Only update display if progress changed (to avoid flickering)
      if (progress != lastProgress) {
        lastProgress = progress;
        
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Translating...");
        display.println("English: " + currentEnglishText);
        display.println("");
        display.print("Progress: ");
        display.print(progress);
        display.println("%");
        
        // Draw progress bar
        int barWidth = 120;
        int progressWidth = (barWidth * progress) / 100;
        display.drawRect(0, 45, barWidth, 10, SSD1306_WHITE);
        display.fillRect(0, 45, progressWidth, 10, SSD1306_WHITE);
        
        display.display();
        
        Serial.println("Progress: " + String(progress) + "%");
      }
      
      // Yellow LED ON during processing
      allLEDsOff();
      digitalWrite(YELLOW_LED, HIGH);
      
    } else {
      // Processing complete - show result
      isTranslating = false;
      lastProgress = -1;
      
      // Green LED ON for result
      allLEDsOff();
      digitalWrite(GREEN_LED, HIGH);
      
      // Show result on OLED
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Translation Complete");
      display.println("");
      display.println("English: " + currentEnglishText);
      
      // Split German text if too long for display
      String displayGerman = currentGermanText;
      if (currentGermanText.length() > 21) { // Approximate characters per line
        // Try to find a space to break the line
        int breakPoint = 21;
        while (breakPoint > 0 && displayGerman.charAt(breakPoint) != ' ') {
          breakPoint--;
        }
        if (breakPoint > 0) {
          displayGerman.setCharAt(breakPoint, '\n');
        }
      }
      
      display.println("German: " + displayGerman);
      display.println("");
      display.println("Press button to reset");
      display.display();
      
      Serial.println("Translation complete:");
      Serial.println("English: " + currentEnglishText);
      Serial.println("German: " + currentGermanText);
    }
  }
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>English to German Translator</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { 
        font-family: Arial, sans-serif; 
        max-width: 600px; 
        margin: 0 auto; 
        padding: 20px; 
        background-color: #f0f0f0;
      }
      .container { 
        background: white; 
        padding: 30px; 
        border-radius: 10px; 
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);
      }
      h1 { 
        color: #2c3e50; 
        text-align: center;
        margin-bottom: 30px;
      }
      .form-group { 
        margin-bottom: 20px; 
      }
      label { 
        display: block; 
        margin-bottom: 5px; 
        font-weight: bold;
        color: #34495e;
      }
      input[type="text"] { 
        width: 100%; 
        padding: 12px; 
        border: 2px solid #bdc3c7; 
        border-radius: 5px; 
        font-size: 16px;
        box-sizing: border-box;
      }
      input[type="text"]:focus {
        border-color: #3498db;
        outline: none;
      }
      button { 
        background: #3498db; 
        color: white; 
        padding: 12px 30px; 
        border: none; 
        border-radius: 5px; 
        cursor: pointer; 
        font-size: 16px;
        width: 100%;
        transition: background 0.3s;
        margin-bottom: 20px;
      }
      button:hover { 
        background: #2980b9; 
      }
      button:disabled {
        background: #bdc3c7;
        cursor: not-allowed;
      }
      .result { 
        margin-top: 20px; 
        padding: 20px; 
        border-radius: 5px; 
        background: #ecf0f1;
        display: none;
      }
      .status { 
        margin-top: 10px; 
        padding: 15px; 
        border-radius: 5px; 
        text-align: center;
        font-weight: bold;
        display: none;
      }
      .processing { 
        background: #f39c12; 
        color: white; 
        display: block;
      }
      .success { 
        background: #27ae60; 
        color: white; 
        display: block;
      }
      .error { 
        background: #e74c3c; 
        color: white; 
        display: block;
      }
      .progress-container {
        margin: 20px 0;
      }
      .progress-bar {
        width: 100%;
        height: 20px;
        background: #ecf0f1;
        border-radius: 10px;
        overflow: hidden;
      }
      .progress-fill {
        height: 100%;
        background: #3498db;
        width: 0%;
        transition: width 0.3s ease;
      }
      .progress-text {
        text-align: center;
        margin-top: 5px;
        font-weight: bold;
      }
      .info-box {
        background: #d5dbdb;
        padding: 15px;
        border-radius: 5px;
        margin-top: 20px;
        font-size: 14px;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>🌍 English to German Translator</h1>
      
      <div class="form-group">
        <label for="englishText">Enter English Text:</label>
        <input type="text" id="englishText" placeholder="Type English words or sentences (e.g., 'today is monday', 'hello how are you')" required>
      </div>
      
      <button id="translateBtn" onclick="translateText()">Translate to German</button>
      
      <div id="progressContainer" class="progress-container" style="display: none;">
        <div class="progress-bar">
          <div id="progressFill" class="progress-fill"></div>
        </div>
        <div id="progressText" class="progress-text">0%</div>
      </div>
      
      <div id="status" class="status"></div>
      
      <div id="result" class="result">
        <h3>Translation Result:</h3>
        <p><strong>English:</strong> <span id="englishResult"></span></p>
        <p><strong>German:</strong> <span id="germanResult"></span></p>
      </div>

      <div class="info-box">
        <strong>Note:</strong> This translator can handle both single words and complete sentences. 
        It translates each word individually and maintains the original sentence structure.
        Progress is synchronized between web browser and OLED display.
      </div>
    </div>

    <script>
      let isProcessing = false;
      let progressInterval;

      function translateText() {
        const text = document.getElementById('englishText').value.trim();
        if (!text) {
          alert('Please enter English text');
          return;
        }

        if (isProcessing) {
          alert('Translation in progress. Please wait...');
          return;
        }

        const statusDiv = document.getElementById('status');
        const resultDiv = document.getElementById('result');
        const progressContainer = document.getElementById('progressContainer');
        const translateBtn = document.getElementById('translateBtn');
        
        // Reset UI
        resultDiv.style.display = 'none';
        statusDiv.className = 'status processing';
        statusDiv.innerHTML = '🔄 Starting translation...';
        statusDiv.style.display = 'block';
        progressContainer.style.display = 'block';
        translateBtn.disabled = true;

        // Reset progress
        updateProgress(0);
        isProcessing = true;

        // Send translation request
        fetch('/translate', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
          body: 'text=' + encodeURIComponent(text)
        })
        .then(response => response.json())
        .then(data => {
          if (data.success) {
            // Start polling for progress
            startProgressPolling(text, data.english, data.german);
          } else {
            handleError('Translation failed: ' + data.error);
          }
        })
        .catch(error => {
          handleError('Network error: ' + error);
        });
      }

      function startProgressPolling(originalText, englishText, germanText) {
        let progressCheckCount = 0;
        const maxChecks = 120; // 4 seconds * 30 checks/second = 120 checks max
        
        progressInterval = setInterval(() => {
          fetch('/progress')
            .then(response => response.json())
            .then(progressData => {
              progressCheckCount++;
              
              if (progressData.translating) {
                // Update progress
                updateProgress(progressData.progress);
                
                // Update status message
                const statusDiv = document.getElementById('status');
                statusDiv.innerHTML = `🔄 Translating... ${progressData.progress}%`;
                
                // If progress is 100% or max checks reached, show result
                if (progressData.progress >= 100 || progressCheckCount >= maxChecks) {
                  clearInterval(progressInterval);
                  showFinalResult(englishText, germanText);
                }
              } else {
                // Translation finished
                clearInterval(progressInterval);
                showFinalResult(englishText, germanText);
              }
            })
            .catch(error => {
              console.error('Progress check error:', error);
              if (progressCheckCount >= maxChecks) {
                clearInterval(progressInterval);
                showFinalResult(englishText, germanText);
              }
            });
        }, 100); // Check every 100ms
      }

      function updateProgress(progress) {
        const progressFill = document.getElementById('progressFill');
        const progressText = document.getElementById('progressText');
        
        progressFill.style.width = progress + '%';
        progressText.textContent = progress + '%';
      }

      function showFinalResult(englishText, germanText) {
        isProcessing = false;
        
        const statusDiv = document.getElementById('status');
        const resultDiv = document.getElementById('result');
        const progressContainer = document.getElementById('progressContainer');
        const translateBtn = document.getElementById('translateBtn');
        
        // Show success status
        statusDiv.className = 'status success';
        statusDiv.innerHTML = '✅ Translation complete!';
        
        // Display results
        document.getElementById('englishResult').textContent = englishText;
        document.getElementById('germanResult').textContent = germanText;
        resultDiv.style.display = 'block';
        
        // Hide progress bar
        progressContainer.style.display = 'none';
        
        // Re-enable button
        translateBtn.disabled = false;
      }

      function handleError(message) {
        isProcessing = false;
        
        const statusDiv = document.getElementById('status');
        const progressContainer = document.getElementById('progressContainer');
        const translateBtn = document.getElementById('translateBtn');
        
        statusDiv.className = 'status error';
        statusDiv.innerHTML = '❌ ' + message;
        progressContainer.style.display = 'none';
        translateBtn.disabled = false;
        
        clearInterval(progressInterval);
      }

      // Allow pressing Enter to translate
      document.getElementById('englishText').addEventListener('keypress', function(e) {
        if (e.key === 'Enter') {
          translateText();
        }
      });

      // Check server status on page load
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          console.log('Server status:', data.status);
        });
    </script>
  </body>
  </html>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

void handleTranslate() {
  if (server.hasArg("text")) {
    String englishText = server.arg("text");
    englishText.toLowerCase();
    englishText.trim();
    
    Serial.println("Translation request for: " + englishText);
    
    // Start translation process
    currentEnglishText = englishText;
    currentGermanText = translateSentence(englishText);
    isTranslating = true;
    processingStartTime = millis();
    lastProgress = 0;
    
    // Red LED ON when waiting for processing
    allLEDsOff();
    digitalWrite(RED_LED, HIGH);
    
    // Update OLED
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("New Translation:");
    display.println("English: " + englishText);
    display.println("");
    display.println("Starting...");
    display.display();
    
    // Return immediate response
    String response = "{";
    response += "\"success\": true,";
    response += "\"english\": \"" + currentEnglishText + "\",";
    response += "\"german\": \"" + currentGermanText + "\"";
    response += "}";
    
    server.send(200, "application/json", response);
    
  } else {
    String response = "{\"success\": false, \"error\": \"No text provided\"}";
    server.send(400, "application/json", response);
  }
}

void handleProgress() {
  if (isTranslating) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - processingStartTime;
    int progress = 0;
    
    if (elapsedTime <= PROCESSING_TIME) {
      progress = (elapsedTime * 100) / PROCESSING_TIME;
    } else {
      progress = 100;
    }
    
    String response = "{";
    response += "\"translating\": true,";
    response += "\"progress\": " + String(progress) + ",";
    response += "\"english\": \"" + currentEnglishText + "\"";
    response += "}";
    
    server.send(200, "application/json", response);
  } else {
    String response = "{\"translating\": false, \"progress\": 0}";
    server.send(200, "application/json", response);
  }
}

void handleStatus() {
  String response = "{\"status\": \"online\", \"translating\": " + String(isTranslating ? "true" : "false") + "}";
  server.send(200, "application/json", response);
}

String translateSentence(String englishSentence) {
  String germanTranslation = "";
  String remainingText = englishSentence;
  remainingText.toLowerCase();
  
  // First, try to find and replace multi-word phrases
  for (int i = 0; i < DICTIONARY_SIZE; i++) {
    String phrase = dictionary[i].english;
    // Only check phrases that have spaces (multi-word phrases)
    if (phrase.indexOf(' ') != -1) {
      int phrasePos = remainingText.indexOf(phrase);
      if (phrasePos != -1) {
        // Replace the phrase in the remaining text
        String beforePhrase = remainingText.substring(0, phrasePos);
        String afterPhrase = remainingText.substring(phrasePos + phrase.length());
        
        // Translate the parts before and after the phrase
        if (beforePhrase.length() > 0) {
          germanTranslation += translateIndividualWords(beforePhrase) + " ";
        }
        
        germanTranslation += dictionary[i].german + " ";
        
        // Update remaining text to be the part after the phrase
        remainingText = afterPhrase;
        
        // Reset the loop to check for other phrases in the remaining text
        i = -1; // Will be incremented to 0 in next iteration
        continue;
      }
    }
  }
  
  // Translate any remaining individual words
  if (remainingText.length() > 0) {
    germanTranslation += translateIndividualWords(remainingText);
  }
  
  germanTranslation.trim();
  return germanTranslation;
}

String translateIndividualWords(String text) {
  String result = "";
  int startPos = 0;
  int spacePos = text.indexOf(' ');
  
  while (spacePos != -1) {
    String word = text.substring(startPos, spacePos);
    word.trim();
    
    if (word.length() > 0) {
      String translatedWord = searchTranslation(word);
      result += translatedWord + " ";
    }
    
    startPos = spacePos + 1;
    spacePos = text.indexOf(' ', startPos);
  }
  
  // Process the last word
  String lastWord = text.substring(startPos);
  lastWord.trim();
  if (lastWord.length() > 0) {
    String translatedWord = searchTranslation(lastWord);
    result += translatedWord;
  }
  
  result.trim();
  return result;
}

String searchTranslation(String englishWord) {
  // First try exact match
  for (int i = 0; i < DICTIONARY_SIZE; i++) {
    if (dictionary[i].english == englishWord) {
      return dictionary[i].german;
    }
  }
  
  // If not found, try case-insensitive match
  for (int i = 0; i < DICTIONARY_SIZE; i++) {
    if (dictionary[i].english.equalsIgnoreCase(englishWord)) {
      return dictionary[i].german;
    }
  }
  
  // If still not found, return the original word with asterisks
  return "*" + englishWord + "*";
}

void resetSystem() {
  isTranslating = false;
  currentEnglishText = "";
  currentGermanText = "";
  lastProgress = -1;
  
  allLEDsOff();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("System Reset");
  display.println("Ready for new");
  display.println("translation");
  display.println("");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
  
  Serial.println("System reset - ready for new translation");
}

void allLEDsOff() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}






