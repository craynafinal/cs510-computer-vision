#define WIN32

#include "FilterBank.h"

void FilterBank::setupButtons(int index, char* label) {
	if (index != -1) {
		// update the specific label
		buttons[index]->label(label);
	}
	else {
		// init all buttons
		buttons[loadButton] = new Fl_Button(10, 10, SIZE_BUTTON_W, SIZE_BUTTON_H, "Load Image");
		buttons[loadButton]->callback(loadButtonCallback, this);
		buttons[selectButton] = new Fl_Button(10, 50, SIZE_BUTTON_W, SIZE_BUTTON_H, "Select Filter");
		buttons[selectButton]->callback(selectButtonCallback, this);
		buttons[selectButton]->deactivate();
		buttons[applyButton] = new Fl_Button(10, 90, SIZE_BUTTON_W, SIZE_BUTTON_H, "Apply Filter");
		buttons[applyButton]->callback(applyButtonCallback, this);
		buttons[applyButton]->deactivate();
	}	
}

void FilterBank::deactivateSliders() {
	for (int i = 0; i < SIZE_SLIDERS; i++) {
		sliders[i]->bounds(0, 1);
		sliders[i]->value(0);
		sliders[i]->label("Slider");
		sliders[i]->deactivate();
	}
}

void FilterBank::setupSliders(char* labels[], int minValues[], int maxValues[], int size) {
	
	if (labels != NULL && minValues != NULL && maxValues != NULL && size != -1) {
		deactivateSliders();
		
		for (int j = 0; j < size; j++) {
			sliders[j]->bounds(minValues[j], maxValues[j]);
			sliders[j]->value(minValues[j]);
			sliders[j]->label(labels[j]);
			sliders[j]->activate();
		}
		redraw();
	}
	else {
		int y = 160;
		for (int i = 0; i < SIZE_SLIDERS; i++) {
			sliders[i] = new Fl_Value_Slider(10, y, SIZE_SLIDER_W, SIZE_SLIDER_H);
			sliders[i]->type(FL_HOR_SLIDER);
			sliders[i]->bounds(0, 1);
			sliders[i]->value(0);
			sliders[i]->label("Slider");
			sliders[i]->deactivate();

			y += SIZE_SLIDER_H + 20;
		}
	}
}

void FilterBank::setupSelector() {
	if (filterSelecter == NULL) {
		filterSelecter = new Fl_Choice(10, 130, 100, 20);

		for (int i = 0; i < sizeof(TEXT_FILTERS) / sizeof(TEXT_FILTERS[0]); i++) {
			filterSelecter->add(TEXT_FILTERS[i]);
		}

		filterSelecter->value(none_filter);
		filterSelecter->deactivate();
	}
	else {
		filterSelecter->active();
	}
}

bool FilterBank::setupImage(char* filepath) {
	bool ret;
	Mat temp = imread(filepath, CV_LOAD_IMAGE_COLOR);
	if (temp.empty()) {
		ret = false;
	}
	else {
		ret = resolveImageFormatAndSaveImage(temp, filepath);
	}

	return ret;
}

bool FilterBank::resolveImageFormatAndSaveImage(Mat img, char* filepath) {
	bool ret;
	
	removeImage();
	string format = ((string)filepath).substr(((string)filepath).length() - 4);
	if (format == ".jpg") {
		saveImage(img, "save.jpg", jpg);
		ret = true;
	}
	else if (format == "jpeg") {
		saveImage(img, "save.jpeg", jpg);
		ret = true;
	}
	else if (format == ".png") {
		saveImage(img, "save.png", png);
		ret = true;
	}
	else if (format == ".bmp") {
		saveImage(img, "save.bmp", bmp);
		ret = true;
	}
	else {
		ret = false;
	}

	return ret;
}

void FilterBank::saveImage(Mat img, char* filename, File_Formats format) {
	
	if (image != NULL) {
		delete image;
	}

	image = new Mat(img);
	savedFilename = filename;
	imwrite((String)savedFilename, *image);

	if (format == jpg)
		setupImagebox(new Fl_JPEG_Image(savedFilename));
	else if (format == png)
		setupImagebox(new Fl_PNG_Image(savedFilename));
	else
		setupImagebox(new Fl_BMP_Image(savedFilename));
}

void FilterBank::loadButtonAction() {
	if (!setupImage(fl_file_chooser("Select a file", "Image Files (*.{bmp,gif,jpg,png})", "", 0))) {
		fl_message("The file is not an image or not selected. Please try again");
	}
	else {
		deactivateSliders();
		buttons[applyButton]->deactivate();

		buttons[selectButton]->activate();
		filterSelecter->activate();
		filterSelecter->value(none_filter);
		selectedFilter = none_filter;
	}
}

void FilterBank::setupImagebox(Fl_Image* img) {
	if (img == NULL) {
		imagebox = new Fl_Box(120, 10, SIZE_IMAGEBOX_W, SIZE_IMAGEBOX_H);
	} 
	else {
		imagebox->image(img);
		redraw();
	}
}

void FilterBank::selectButtonAction() {

	switch (filterSelecter->value()) {
	case none_filter:
		fl_message("Please select a filter");
		break;
	case box_filter:
		setupSliders(new char*[]{ "Depth", "Kernel X", "Kernel Y", "Anchor X", "Anchor Y" }, new int[] { -1, 0, 0, -1, -1 }, new int[] { 99, 99, 99, 99, 99 }, 5);
		buttons[applyButton]->activate();
		selectedFilter = box_filter;
		break;
	case gaussian_filter:
		setupSliders(new char*[]{ "Kernel X", "Kernel Y", "Sigma X", "Sigma Y" }, new int[] { 0, 0, 0, 0 }, new int[] { 100, 100, 100, 100 }, 4);
		buttons[applyButton]->activate();
		selectedFilter = gaussian_filter;
		break;
	case median_filter:
		setupSliders(new char*[]{ "Kernel Size" }, new int[] { 1 }, new int[] { 99 }, 1);
		buttons[applyButton]->activate();
		selectedFilter = median_filter;
		break;
	case bilateral_filter:
		setupSliders(new char*[]{ "Diameter", "Sigma Color", "Sigma Space" }, new int[] { 0, 0, 0 }, new int[] { 9, 200, 200 }, 3);
		buttons[applyButton]->activate();
		selectedFilter = bilateral_filter;
		break;
	case canny_edge:
		setupSliders(new char*[]{ "Low Thresh", "High Thresh" }, new int[] { 1, 1 }, new int[] { 200, 200 }, 2);
		buttons[applyButton]->activate();
		selectedFilter = canny_edge;
		break;
	case nonlocal_filter:
		setupSliders(new char*[]{ "Luminance" }, new int[] { 0 }, new int[] { 100 }, 1);
		buttons[applyButton]->activate();
		selectedFilter = nonlocal_filter;
		break;
	case despeckle:
		setupSliders(new char*[]{ "Kernel X", "Kernel Y" }, new int[] { 0, 0 }, new int[] { 99, 99 }, 2);
		buttons[applyButton]->activate();
		selectedFilter = despeckle;
		break;
	case de2_filter:
		setupSliders(new char*[]{ "Threshold" }, new int[] { 0 }, new int[] { 1 }, 1);
		buttons[applyButton]->activate();
		selectedFilter = de2_filter;
		break;
	default:
		fl_message("Error detected, please try again");
	}
}

void FilterBank::applyButtonAction() {

	Mat dst;
	Mat bgr[3];
	double minVal;
	double maxVal;
	int max_dim;
	int padded_width;
	int padded_height;

	int sliderValues[SIZE_SLIDERS];

	switch (selectedFilter) {
	case none_filter:
		fl_message("Please select a filter");
		break;
	case box_filter:
		sliderValues[0] = (int)(sliders[0]->value());
		sliderValues[1] = (int)(sliders[1]->value());
		sliderValues[2] = (int)(sliders[2]->value());
		sliderValues[3] = (int)(sliders[3]->value());
		sliderValues[4] = (int)(sliders[4]->value());


		sliderValues[1] = sliderValues[1] % 2 == 0 ? sliderValues[1] + 1 : sliderValues[1];
		sliderValues[2] = sliderValues[2] % 2 == 0 ? sliderValues[2] + 1 : sliderValues[2];

		if ((sliderValues[3] > sliderValues[1]) || (sliderValues[4] > sliderValues[2])) {
			fl_message("Please enter a correct value");
		}
		else {
			boxFilter(*image, dst, sliderValues[0], Size(sliderValues[1], sliderValues[2]), Point(sliderValues[3], sliderValues[4]));
			resolveImageFormatAndSaveImage(dst, savedFilename);
		}

		break;
	case gaussian_filter:
		sliderValues[0] = (int)(sliders[0]->value());
		sliderValues[1] = (int)(sliders[1]->value());
		sliderValues[2] = (int)(sliders[2]->value());
		sliderValues[3] = (int)(sliders[3]->value());

		if (!(sliderValues[0] == 0 && sliderValues[1] == 0)) {
			sliderValues[0] = sliderValues[0] % 2 == 0 ? sliderValues[0] + 1 : sliderValues[0];
			sliderValues[1] = sliderValues[1] % 2 == 0 ? sliderValues[1] + 1 : sliderValues[1];
		}
		
		if (((sliderValues[0] == 0 || sliderValues[1] == 0) && sliderValues[2] == 0 && sliderValues[3] == 0) || (sliderValues[0] == 0 && sliderValues[1] == 0 && sliderValues[2] == 0 && sliderValues[3] != 0)) {
			fl_message("Please enter a correct value");
		}
		else {
			GaussianBlur(*image, dst, Size(sliderValues[0], sliderValues[1]), sliderValues[2], sliderValues[3]);
			resolveImageFormatAndSaveImage(dst, savedFilename);
		}

		break;
	case median_filter:
		sliderValues[0] = (int)(sliders[0]->value());
		sliderValues[0] = sliderValues[0] % 2 == 0 ? sliderValues[0] + 1 : sliderValues[0];

		medianBlur(*image, dst, sliderValues[0]);
		resolveImageFormatAndSaveImage(dst, savedFilename);
		break;
	case bilateral_filter:
		sliderValues[0] = (int)(sliders[0]->value());
		sliderValues[1] = (int)(sliders[1]->value());
		sliderValues[2] = (int)(sliders[2]->value());

		bilateralFilter(*image, dst, sliderValues[0], sliderValues[1], sliderValues[2]);
		resolveImageFormatAndSaveImage(dst, savedFilename);
		break;
	case canny_edge:
		sliderValues[0] = (int)(sliders[0]->value());
		sliderValues[1] = (int)(sliders[1]->value());

		if (sliderValues[1] <= sliderValues[0]) {
			fl_message("Please enter a correct value");
		}
		else {
			Canny(*image, dst, sliderValues[0], sliderValues[1], 3);
			resolveImageFormatAndSaveImage(dst, savedFilename);
		}

		break;
	case nonlocal_filter:
		sliderValues[0] = (int)(sliders[0]->value());

		fastNlMeansDenoisingColored(*image, dst, sliderValues[0]);
		resolveImageFormatAndSaveImage(dst, savedFilename);
		break;
	case despeckle:
		sliderValues[0] = sliders[0]->value();
		sliderValues[1] = sliders[1]->value();

		sliderValues[0] = sliderValues[0] % 2 == 0 ? sliderValues[0] + 1 : sliderValues[0];
		sliderValues[1] = sliderValues[1] % 2 == 0 ? sliderValues[1] + 1 : sliderValues[1];

		morphologyEx(*image, dst, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(sliderValues[0], sliderValues[1])));
		morphologyEx(dst, dst, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(sliderValues[0], sliderValues[1])));

		minMaxLoc(dst, &minVal, &maxVal, NULL, NULL);
		dst -= minVal;
		dst.convertTo(dst, -1, (256 / (maxVal - minVal)));

		resolveImageFormatAndSaveImage(dst, savedFilename);
		break;
	case de2_filter:
		max_dim = max(image->rows, image->cols);
		
		if (max_dim > 1024) {
			cv::resize(*image, *image, Size(image->cols* 1024 / max_dim, image->rows * 1024 / max_dim));
		}
		
		padded_width = pow(2, ceil(log(image->cols) / log(2)));
		padded_height = pow(2, ceil(log(image->rows) / log(2)));

		copyMakeBorder(*image, *image, 0, padded_height - image->rows, 0, padded_width - image->cols, BORDER_CONSTANT);

		split(*image, bgr);

		Denoise(bgr[0].clone(), bgr[0], sliders[0]->value());
		Denoise(bgr[1].clone(), bgr[1], sliders[0]->value());
		Denoise(bgr[2].clone(), bgr[2], sliders[0]->value());

		merge(bgr, 3, dst);

		resolveImageFormatAndSaveImage(dst, savedFilename);
		break;
	default:
		fl_message("Error detected, please try again");
	}
}

void FilterBank::removeImage() {
	if (savedFilename != "") {
		std::remove(savedFilename);
	}
}

FilterBank::FilterBank(int width, int height, char* title) : Fl_Window(width, height, title) {
	setupButtons();
	setupSliders();
	setupSelector();
	setupImagebox();
}

FilterBank::~FilterBank() {
	removeImage();
}

void loadButtonCallback(Fl_Widget* object, void* data) {
	((FilterBank*)data)->loadButtonAction();
}

void selectButtonCallback(Fl_Widget* object, void* data) {
	((FilterBank*)data)->selectButtonAction();
}

void applyButtonCallback(Fl_Widget* object, void* data) {
	((FilterBank*)data)->applyButtonAction();
}










void HaarDecomposeArray(float *A, int width)
{
	const float inv_sqrt2 = 1 / sqrt(2);

	float norm = 1.0f / sqrt(width);

	for (int i = 0; i < width; i++) {
		A[i] *= norm;
	}

	float *tmp = new float[width];

	while (width > 1) {
		width /= 2;

		for (int i = 0; i < width; i++) {
			tmp[i] = (A[2 * i] + A[2 * i + 1]) * inv_sqrt2;
			tmp[width + i] = (A[2 * i] - A[2 * i + 1]) * inv_sqrt2;
		}

		memcpy(A, tmp, width * 2 * sizeof(float));
	}

	delete[] tmp;
}

void HaarTransposeArray(float *A, int width, int height)
{
	float *B = new float[width*height];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			B[x*height + y] = A[y*width + x];
		}
	}

	memcpy(A, B, sizeof(float)*width*height);

	delete[] B;
}

void HaarDecomposeImage(float *A, int width, int height)
{
	for (int i = 0; i < height; i++) {
		HaarDecomposeArray(&A[i*width], width);
	}

	HaarTransposeArray(A, width, height);

	for (int i = 0; i < width; i++) {
		HaarDecomposeArray(&A[i*height], height);
	}

	HaarTransposeArray(A, height, width);
}

void HaarReconstructArray(float *A, int width)
{
	const float inv_sqrt2 = 1 / sqrt(2);
	float inv_norm = sqrt(width);

	float *tmp = new float[width * 2];
	int k = 1;

	while (k < width)  {
		for (int i = 0; i < k; i++) {
			tmp[2 * i] = (A[i] + A[k + i]) * inv_sqrt2;
			tmp[2 * i + 1] = (A[i] - A[k + i]) * inv_sqrt2;
		}

		memcpy(A, tmp, sizeof(float)*(k * 2));

		k *= 2;
	}

	for (int i = 0; i < width; i++) {
		A[i] *= inv_norm;
	}

	delete[] tmp;
}

void HaarReconstructImage(float *A, int width, int height)
{
	for (int i = 0; i < width; i++) {
		HaarReconstructArray(&A[i*height], height);
	}

	HaarTransposeArray(A, height, width);

	for (int i = 0; i < height; i++) {
		HaarReconstructArray(&A[i*width], width);
	}

	HaarTransposeArray(A, width, height);
}

void Denoise(const Mat &input, Mat &output, float threshold)
{
	assert(input.data != output.data);

	input.convertTo(output, CV_32F);

	HaarDecomposeImage((float*)output.data, output.cols, output.rows);

	for (int y = 0; y < output.rows; y++) {
		float *ptr = (float*)output.ptr(y);

		for (int x = 0; x < output.cols; x++) {
			// signbit returns 1 for negative numbers, and 0 otherwise
			// Haar wavelet shrinkage using soft thresholding
			ptr[x] = (signbit(ptr[x]) == 1 ? -1 : 1) * max(0.f, fabs(ptr[x]) - threshold);
		}
	}

	HaarReconstructImage((float*)output.data, output.rows, output.cols);

	output.convertTo(output, CV_8U);
}