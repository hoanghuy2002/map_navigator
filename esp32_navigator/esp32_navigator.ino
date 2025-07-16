
#include <ChronosESP32.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include"ImagesDirections.h"
#include"ImagesLanes.h"
#include"ImagesOther.h"
#include"DataConstants.h"
#include"ImageProccess.h"
#include "FontMaker.h"

#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define BLACK  0x0000
#define WHITE 0xFFFF
MakeFont myfont(&setpx);

ChronosESP32 watch("ChauEC1"); // set the bluetooth name
// Cấu hình màn hình
TFT_eSPI tft = TFT_eSPI();
void setpx(int16_t x, int16_t y, uint16_t color)
{
  tft.drawPixel(x, y, color); //Thay đổi hàm này thành hàm vẽ pixel mà thư viện led bạn dùng cung cấp
}

// Định nghĩa kích thước màn hình
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define HALF_HEIGHT 120

// Con trỏ cho hai buffer được cấp phát động
uint16_t* buffer1 = NULL; // Buffer cho nửa trên màn hình
uint16_t* buffer2 = NULL; // Buffer cho nửa dưới màn hình


bool change = false;
uint8_t direct = 0 ;
//String navigate =  
void Draw4bitImageProgmem(int x, int y, int width, int height, const uint8_t* pBmp);
void SetPixelCanvas(int16_t x, int16_t y, uint16_t value);
void Draw565ImageProgmem(int x, int y, int width, int height, const uint16_t* pBmp);

void connectionCallback(bool state)
{
  Serial.print("Connection state: ");
  Serial.println(state ? "Connected" : "Disconnected");
}

// Hàm cấp phát động cho các buffer
bool InitBuffers() {
  // Giải phóng buffer cũ nếu đã tồn tại
  if (buffer1 != NULL) {
    free(buffer1);
    buffer1 = NULL;
  }

  if (buffer2 != NULL) {
    free(buffer2);
    buffer2 = NULL;
  }

  // Cấp phát buffer mới
  buffer1 = (uint16_t*)malloc(SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));
  buffer2 = (uint16_t*)malloc(SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));

  // Kiểm tra xem cấp phát có thành công không
  if (buffer1 == NULL || buffer2 == NULL) {
    // Xử lý lỗi - giải phóng bất kỳ bộ nhớ nào đã được cấp phát
    if (buffer1 != NULL) {
      free(buffer1);
      buffer1 = NULL;
    }

    if (buffer2 != NULL) {
      free(buffer2);
      buffer2 = NULL;
    }

    Serial.println("Failed to allocate memory for buffers");
    return false;
  }

  // Khởi tạo buffer với giá trị 0
  memset(buffer1, 0, SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));
  memset(buffer2, 0, SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));

  return true;
}

// Hàm giải phóng bộ nhớ buffer
void FreeBuffers() {
  if (buffer1 != NULL) {
    free(buffer1);
    buffer1 = NULL;
  }

  if (buffer2 != NULL) {
    free(buffer2);
    buffer2 = NULL;
  }
}

// Hàm thiết lập pixel trên buffer thích hợp
void SetPixelCanvas(int16_t x, int16_t y, uint16_t color) {
  // Kiểm tra xem các buffer đã được cấp phát chưa
  if (buffer1 == NULL || buffer2 == NULL) return;

  // Kiểm tra tọa độ hợp lệ
  if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;

  if (y < HALF_HEIGHT) {
    buffer1[y * SCREEN_WIDTH + x] = color;
  } else {
    buffer2[(y - HALF_HEIGHT) * SCREEN_WIDTH + x] = color;
  }
}

// Hàm vẽ ảnh 4-bit từ PROGMEM vào các buffer
void Draw4bitImageProgmem(int x, int y, int width, int height, const uint8_t* pBmp)
{
  // check if the buffers are allocated?
  if (buffer1 == NULL || buffer2 == NULL) return;
  const int sizePixels = width * height;
  for (int i = 1; i < sizePixels; i += 2)
  {
    uint8_t data = pgm_read_byte(pBmp++);
    uint8_t leftPixel = (data & 0x0F);
    uint8_t rightPixel = (data & 0xF0) >> 4;

    int yLeft = y + (i - 1) / width;
    int xLeft = x + (i - 1) % width;

    int yRight = y + i / width;
    int xRight = x + i % width;

    SetPixelCanvas(xLeft, yLeft, Color4To16bit(leftPixel));
    SetPixelCanvas(xRight, yRight, Color4To16bit(rightPixel));
  }
}

void Draw565ImageProgmem(int x, int y, int width, int height, const uint16_t* pBmp)
{
  // check if the buffers are allocated?
  if (buffer1 == NULL || buffer2 == NULL) return;
  const int sizePixels = width * height;
  for (int i = 0; i < sizePixels; i ++)
  {

    if (i < HALF_HEIGHT * SCREEN_WIDTH) {
      buffer1[i] =  pgm_read_word(&(pBmp[i]));
    } else {
      buffer2[i - HALF_HEIGHT * SCREEN_WIDTH] =  pgm_read_word(&(pBmp[i]));
    }

  }
}


void DisplayBuffers() {
  // check if the buffers are allocated?
  if (buffer1 == NULL || buffer2 == NULL) return;

  // display half above of screen
  tft.setAddrWindow(0, 0, SCREEN_WIDTH, HALF_HEIGHT);
  tft.pushColors(buffer1, SCREEN_WIDTH * HALF_HEIGHT);

  // display half bottom of screen
  tft.setAddrWindow(0, HALF_HEIGHT, SCREEN_WIDTH, HALF_HEIGHT);
  tft.pushColors(buffer2, SCREEN_WIDTH * HALF_HEIGHT);
}

// Hàm xóa toàn bộ buffer (màu đen)
void ClearBuffers() {
  // Kiểm tra xem các buffer đã được cấp phát chưa
  if (buffer1 == NULL || buffer2 == NULL) return;

  memset(buffer1, 0, SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));
  memset(buffer2, 0, SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));
}


void notificationCallback(Notification notification)
{
  Serial.print("Notification received at ");
  Serial.println(notification.time);
  Serial.print("From: ");
  Serial.print(notification.app);
  Serial.print("\tIcon: ");
  Serial.println(notification.icon);
  Serial.println(notification.title);
  Serial.println(notification.message);
}

void clearBuff() {
  memset(buffer1, 0 , SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));
  memset(buffer2, 0 , SCREEN_WIDTH * HALF_HEIGHT * sizeof(uint16_t));

}


void configCallback(Config config, uint32_t a, uint32_t b)
{
  uint8_t flag = 0;

  switch (config)
  {
    case CF_NAV_DATA:
      Serial.print("Navigation state: ");
      Serial.println(a ? "Active" : "Inactive");
      change = true;

      if (a) {
        Navigation nav = watch.getNavigation();
      } 
      break;
    case CF_NAV_ICON:
      Serial.print("Direct code:");
      Serial.println(b);
      direct = convertImage(b);
      break;
  }
}

void setupDisplay() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
}


void animate_instruction(int y_position, uint16_t font_color ,uint16_t background_color, String instruction, uint16_t instruction_length)
{
  static int x = 20;
  myfont.print(x, y_position, instruction, font_color, background_color);
  delay(30);
  myfont.print(x, y_position, instruction, background_color, background_color);

  x -= 3;
  if (x <= 200 - instruction_length)
  {
    x = 20;
  }
  
}


void setup()
{

  Serial.begin(115200);
  Serial.println("Start Program");

  // Cấp phát bộ nhớ cho các buffer
  if (!InitBuffers()) {
    Serial.println("Buffer initialization failed!");
    return;
  }
  setupDisplay();
  // set the callbacks before calling begin funtion
  watch.setConnectionCallback(connectionCallback);
  watch.setNotificationCallback(notificationCallback);
  watch.setConfigurationCallback(configCallback);
  myfont.set_font(VN);
  watch.begin(); // initializes the BLE
  // make sure the ESP32 is not paired with your phone in the bluetooth settings
  // go to Chronos app > Watches tab > Watches button > Pair New Devices > Search > Select your board
  // you only need to do it once. To disconnect, click on the rotating icon (Top Right)

  Serial.println(watch.getAddress()); // mac address, call after begin()

  watch.setBattery(80); 

  Draw565ImageProgmem(0, 0, 240, 240, IMG_GGMap);   // display default background
  DisplayBuffers();
  
}


void loop()
{
  static String instruction;
  static uint16_t instruction_length;
  static bool animation_instruction_Flag = false;

  watch.loop(); // handles internal routine functions


  if (animation_instruction_Flag == true)
  {
    animate_instruction(128, WHITE, BLACK, instruction, instruction_length);
  }
  
  if (change) 
  {
    change = false;

    Navigation nav = watch.getNavigation();
    if (nav.active) 
    {
      uint16_t length = 0;
      clearBuff();
      DisplayBuffers();
      if(direct>0&&direct <40)
      {
        Draw4bitImageProgmem(64, 64, 64, 64, ImageFromDirection(direct));
        DisplayBuffers();
      }

      instruction_length = myfont.getLength(nav.directions);
      if (instruction_length <= 230)
      {
        animation_instruction_Flag = false;                 // Disable animation of instruction
        myfont.print(120 - instruction_length/2, 128, nav.directions, WHITE, BLACK); // display "Instruction by words"
      }
      else
      {
        animation_instruction_Flag = true;                 // Enable animation of instruction
        instruction = nav.directions;
      }

      myfont.print(140, 96, nav.title, WHITE, BLACK);     // distance of current instruction

      length = myfont.getLength(nav.distance);
      myfont.print(120 - length/2, 1, nav.distance, WHITE, BLACK);    // distance from current position to target

      length = myfont.getLength(nav.duration);
      myfont.print(120 - length/2, 190, nav.duration, WHITE, BLACK);    // time to reach target
      
      // Serial.println(nav.directions);
    }
    else
    {
      animation_instruction_Flag = false;
      clearBuff();
      Draw565ImageProgmem(0, 0, 240, 240, IMG_GGMap);
      DisplayBuffers();
    }
  }

}