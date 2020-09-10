# jomjol-tflite-wrapper
Here the essentially part of a c++ wrapper class for the Tensorfloor Tflite for an ESP32 is demonstrated. The basic idea or task, which is triggering my activity is coming from an AI-on-the-edge device, also published in Github: https://github.com/jomjol/AI-on-the-edge-device/wiki

But as the essential part on the Tflite integration is deeply covered inside the surrounding code, I have decided to polish one of my testing routines as an example on how to use the wrapper.

I started from the c++ tflite library inside Tensorflow and wanted an easy customized implementation, with the following properties:
* easy to handle on a ESP32 system
* capability to run **models bigger than 1 MByte**
* ideally wrapped into a c++ class to be better capsuled for my project
* **direct loading of tflite-files from a SD-card**, to separate the code from the content/task and much leaner deployment of modifications (code and CNN wise)

Therefore the basic extract of this idea can be found in the attached library. It consists of two parts:

1.  Tensorflow Tflite library [/code/lib/tfmicro](./code/lib/tfmicro) - with modification to increase the possible model size
2. CTfLiteClass as the c++ wrapper handling tflite loading, input loading and readout. This can be found here: [/code/lib/jomjol_tfliteclass](./code/lib/jomjol_tfliteclass)

The basic usage for a classification network is as easy as follows:

        classtflite = new CTfLiteClass; 
        classtflite->LoadModel("cnn_file.tflite"); 
        classtflite->MakeAllocate();  
        classtflite->LoadInputImage("input.bmp");
        classtflite->Invoke();
        result = classtflite->GetOutClassification();
        delete classtflite;



The surrounding code is implemented in the ESP32 Espressif SDK using the PlatformIO development environment.

To run this you need a ESP32 with SD-card and PSRAM. The  additional PSRAM is needed to run the big models I use (even in tflite coding > 1MByte). I tested this code on a ESP32CAM (the same than in the main project. On the SD-card you need to copy the content of [/sd-card](./sd-card) to the root directory.



In the main routine you find two testing loops for two types of models with iterating tflite files, which are loaded one after the other and tested on 5 test images each. The time needed is written to the serial interface.



The following CNN-files are tested. The details to the CNN and the task can be found following the above link. This repository is only to extract the very essential part of the tflite and not the neural network background.



##### 1. CNN digit

This is a classification network, that is used to recognize digits out of an image and transfer into the digits 0 ... 9, including a 10th class for NaN ("Not a Number").
##### 2. CNN analog
This is also a CNN network with two output neurons, which encode as the value of a sinus and cosinus number the angle of a pointer of the input image.

