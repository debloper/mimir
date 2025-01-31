#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "wifi-ssid";
const char* password = "wifi-password";

// Setup upload server with `pip install uploadserver`
// then run `uploadserver` to start the upload server
// Update the following details with your preferences
const char* server = "http://<server-IP>:8000/upload";
const char* content = "Hello, World!";
const char* filename = "example.txt";

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" CONNECTED!");

  // Process buffer to upload
  const uint8_t* buffer = (const uint8_t*)content;
  size_t size = strlen(content);

  // Upload file to the server
  if (uploadFileToServer(server, buffer, size, filename)) { Serial.println("Uploaded to server."); }
  else { Serial.println("Failed to upload to server."); }

  Serial.println("OK");
}

bool uploadFileToServer(const char* url, const uint8_t* buffer, size_t size, const char* filename) {
  HTTPClient http;
  Serial.printf("Uploading to URL: %s\n", url);
  http.begin(url);

  String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  String contentType = "multipart/form-data; boundary=" + boundary;
  http.addHeader("Content-Type", contentType);

  String body = "--" + boundary + "\r\n";
  body += "Content-Disposition: form-data; name=\"files\"; filename=\"" + String(filename) + "\"\r\n";
  body += "Content-Type: text/plain\r\n\r\n";
  body += String((const char*)buffer, size) + "\r\n";
  body += "--" + boundary + "--\r\n";

  int httpResponseCode = http.POST((uint8_t*)body.c_str(), body.length());

  if (httpResponseCode > 0 && httpResponseCode < 400) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    return true;
  } else {
    Serial.printf("Error code: %d\n", httpResponseCode);
    Serial.printf("Error message: %s\n", http.errorToString(httpResponseCode).c_str());
    return false;
  }

  http.end();
}

void loop() {}
