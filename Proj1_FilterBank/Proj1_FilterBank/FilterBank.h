#ifndef FILTERBANK_H
#define FILTERBANK_H
#define WIN32

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <Fl/Fl_Choice.H>
#include <Fl/Fl_Ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_BMP_Image.H>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

static const int SIZE_WINDOW_W = 800;
static const int SIZE_WINDOW_H = 600;
static const int SIZE_BUTTON_W = 100;
static const int SIZE_BUTTON_H = 30;
static const int SIZE_SLIDER_W = 100;
static const int SIZE_SLIDER_H = 20;
static const int SIZE_IMAGEBOX_W = 640;
static const int SIZE_IMAGEBOX_H = 480;

static const int SIZE_BUTTONS = 3;
static const int SIZE_SLIDERS = 5;

enum Index_Buttons	{	loadButton,
						selectButton,
						applyButton
					};

enum Index_Filters	{	none_filter,
						box_filter,
						gaussian_filter, 
						median_filter, 
						bilateral_filter, 
						canny_edge, 
						nonlocal_filter, 
						despeckle, 
						de2_filter
					};

enum File_Formats	{	jpg,
						png,
						bmp
					};

static const char* TEXT_FILTERS[] = {	"Choose Filter",
										"Box Filter",
										"Gaussian Filter",
										"Median Filter",
										"Bilateral Filter",
										"Canny Edge Detector",
										"Non-local Mean Filter",
										"Despeckle (Extra 1)",
										"Haar Wavelet Denoise (Extra 2)"
									};

class FilterBank : public Fl_Window
{
public:
	FilterBank(int width, int height, char* title);
	virtual ~FilterBank();
	void loadButtonAction();
	void selectButtonAction();
	void applyButtonAction();
private:
	Fl_Button* buttons[SIZE_BUTTONS];
	Fl_Value_Slider* sliders[SIZE_SLIDERS];
	Fl_Choice* filterSelecter = NULL;
	Fl_Box* imagebox = NULL;
	Mat* image = NULL;
	int selectedFilter = -1;
	char* savedFilename = "";

	void setupButtons(int index = -1, char* label = NULL);
	void setupSliders(char* labels[] = NULL, int minValues[] = NULL, int maxValues[] = NULL, int size = -1);
	void setupSelector();
	bool setupImage(char* filepath);
	void setupImagebox(Fl_Image* img = NULL); //
	void saveImage(Mat img, char* filename, File_Formats format);
	void removeImage();
	void deactivateSliders();
	bool resolveImageFormatAndSaveImage(Mat img, char* filepath);
};

void loadButtonCallback(Fl_Widget* object, void* data);
void selectButtonCallback(Fl_Widget* object, void* data);
void applyButtonCallback(Fl_Widget* object, void* data);

void HaarReconstructImage(float *A, int width, int height);
void Denoise(const Mat &input, Mat &output, float threshold);
void HaarReconstructArray(float *A, int width);
void HaarDecomposeImage(float *A, int width, int height);
void HaarTransposeArray(float *A, int width, int height);
void HaarDecomposeArray(float *A, int width);

#endif