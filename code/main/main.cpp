#include <stdio.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"

#include "CTfLiteClass.h"


static const char *TAGMAIN = "main";
#define FLASH_GPIO GPIO_NUM_4

void Init_SDCard_GPIO()
{
    ESP_LOGI(TAGMAIN, "Initializing SD card");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.flags = SDMMC_HOST_FLAG_1BIT;    
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = { };
    mount_config.format_if_mount_failed = false;
    mount_config.max_files = 5;

    sdmmc_card_t* card;
    esp_err_t ret;
    ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAGMAIN, "Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAGMAIN, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);
        }
        return;
    }
    sdmmc_card_print_info(stdout, card);

	// Init the GPIO and turn of flash light (sometime switched on after startup)
    gpio_pad_select_gpio(FLASH_GPIO);
    gpio_set_direction(FLASH_GPIO, GPIO_MODE_OUTPUT);  
    gpio_set_level(FLASH_GPIO, 0);   

}

void test_common_model()
{
    CTfLiteClass *classtflite, *classtflite2;

    float result;
    uint32_t starttime, stoptime, loadtime, calctime, loadtime_sum, calctime_sum;
    std::string name_tflite, name_image;
    int tflite, image;
    int num_tflite = 4;
    float f1, f2;    


    starttime = esp_log_timestamp();

    classtflite = new CTfLiteClass; 
    classtflite2 = new CTfLiteClass;

    name_tflite = "/sdcard/digit0.tflite";
    classtflite->LoadModel(name_tflite); 
    classtflite->MakeAllocate();     

    name_tflite = "/sdcard/analog0.tflite";
//    classtflite2->LoadModel(name_tflite); 


//    classtflite2->MakeAllocate();

    stoptime = esp_log_timestamp();
    printf("Model loading time: %dms\n  ", stoptime-starttime);
    loadtime_sum = 0; 
    calctime_sum = 0;
    starttime = stoptime;

    name_image = "/sdcard/digit1.bmp";
    classtflite->LoadInputImage(name_image);
    stoptime = esp_log_timestamp(); 
    loadtime = stoptime - starttime; starttime = stoptime;
    classtflite->Invoke();
    stoptime = esp_log_timestamp(); 
    calctime = stoptime - starttime; starttime = stoptime;
    result = classtflite->GetOutClassification();
    loadtime_sum += loadtime;
    calctime_sum += calctime;
    printf("Image: %s - CNN-Classification: %d - Loadtime: %dms, Calctime: %dms\n  ", name_image.c_str(), (int) result, loadtime, calctime);

/*
    name_image = "/sdcard/pointer1.bmp";
    classtflite2->LoadInputImage(name_image);

    stoptime = esp_log_timestamp(); 
    loadtime = stoptime - starttime; starttime = stoptime;

    classtflite2->Invoke();

    stoptime = esp_log_timestamp(); 
    calctime = stoptime - starttime; starttime = stoptime;

    f1 = classtflite2->GetOutputValue(0);
    f2 = classtflite2->GetOutputValue(1);
    result = fmod(atan2(f1, f2) / (M_PI * 2) + 2, 1) * 10;
    loadtime_sum += loadtime;
    calctime_sum += calctime;
    printf("Image: %s - CNN-Result: %f - Loadtime: %dms, Calctime: %dms\n  ", name_image.c_str(), result, loadtime, calctime);
*/

}

/*
void test_analog_model()
    {
//    CTfLiteClass *classtflite;

    float f1, f2, result;
    uint32_t starttime, stoptime, loadtime, calctime, loadtime_sum, calctime_sum;
    std::string name_tflite, name_image;
    int tflite, image;
    int num_tflite = 5;

    for (tflite = 0; tflite < num_tflite; ++tflite)
    {
        name_tflite = "/sdcard/analog" + std::to_string(tflite) + ".tflite";
        printf("\n====== Model analog%d.tflite ====================\n  ", tflite);
        starttime = esp_log_timestamp();
        classtflite2 = new CTfLiteClass; 
        classtflite2->LoadModel(name_tflite); 
        classtflite2->MakeAllocate();  
        stoptime = esp_log_timestamp();
        printf("Model loading time: %dms\n  ", stoptime-starttime);
        loadtime_sum = 0; 
        calctime_sum = 0;
        starttime = stoptime;

        for (image = 0; image < 5; ++image)
        {
            name_image = "/sdcard/pointer" + std::to_string(image) + ".bmp";
            classtflite2->LoadInputImage(name_image);

            stoptime = esp_log_timestamp(); 
            loadtime = stoptime - starttime; starttime = stoptime;

            classtflite2->Invoke();

            stoptime = esp_log_timestamp(); 
            calctime = stoptime - starttime; starttime = stoptime;

            f1 = classtflite2->GetOutputValue(0);
            f2 = classtflite2->GetOutputValue(1);
            result = fmod(atan2(f1, f2) / (M_PI * 2) + 2, 1) * 10;
            loadtime_sum += loadtime;
            calctime_sum += calctime;
            printf("Image: %s - CNN-Result: %f - Loadtime: %dms, Calctime: %dms\n  ", name_image.c_str(), result, loadtime, calctime);
        }

        printf("Average Loadtime: %dms, Calctime: %dms\n", (int) (loadtime_sum/5),  (int) (calctime_sum/5));

//        delete classtflite;

    }
}
*/

void doTask(void *pvParameter)
    {
        printf("\n\n========= COMMON MODELS ==================================================\n");
        test_common_model();

        printf("\n\n\nTest is finished --> going to infite loop until hard reboot.\n");
        
        while (1) {
            vTaskDelay(10);
        };
    }

extern "C" void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_ERROR);
    Init_SDCard_GPIO();

    xTaskCreate(&doTask, "doTask", configMINIMAL_STACK_SIZE * 64, NULL, 5, NULL);
}
