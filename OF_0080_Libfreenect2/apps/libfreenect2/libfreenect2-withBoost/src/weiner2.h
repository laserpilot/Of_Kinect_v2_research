//
//  weiner2.h
//  kinectExample
//
//  Created by Johnny Weiner on 3/19/14.
//
//

#include <boost/program_options.hpp>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;
namespace po = boost::program_options;


Mat wiener2(Mat I, Mat image_spectrum, int noise_stddev);
Mat padd_image(Mat I);

Mat get_spectrum(Mat I);
Mat get_dft(Mat I);

Mat with_noise(Mat image, int stddev);
Mat rand_noise(Mat I, int stddev);


/*
int main(int argc, char *argv[]) {
    
	int noise_stddev;
	string input_filename, output_filename;
    
	po::options_description desc("Allowed options");
	desc.add_options()
    ("help", "produce help message")
    ("noise-stddev, n", po::value<int>(&noise_stddev)->default_value(50), "set white noise standard deviation")
    ("input, f", po::value<string>(&input_filename))
    ("output, o", po::value<string>(&output_filename)->default_value(string("output.bmp")),"output file")
    ("generate-noisy", "generate noisy image")
    ("show", "shows effects of filtering")
    ;
    
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
    
	cout << "noise standard deviation: " << noise_stddev << "\n";
	cout << "input file: " << input_filename << "\n";
    
	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}
    
	Mat I = imread(input_filename, CV_LOAD_IMAGE_GRAYSCALE);
	if(I.data==NULL){
		cout << "Can't open file: " << input_filename << "\n";
		return 2;
	}
    
	Mat raw_sample = imread("sample.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	if(raw_sample.data==NULL){
		cout << "Can't open file: sample.bmp\n";
		return 3;
	}
    
	Mat padded = padd_image(I);
	Mat noisy;
	if(vm.count("generate-noisy")){
		noisy = with_noise(padded, noise_stddev);
		imwrite(output_filename, noisy);
		return 0;
	}else{
		noisy = padded;
	}
    
	Mat sample(padded.rows, padded.cols, CV_8U);
	resize(raw_sample, sample, sample.size());
	Mat spectrum = get_spectrum(sample);
	Mat enhanced = wiener2(noisy, spectrum, noise_stddev);
    
	imwrite(output_filename, enhanced);
    
	if(vm.count("show")){
		imshow("image 1", noisy);
		imshow("image 2", enhanced);
	}
	waitKey();
}
*/

Mat wiener2(Mat I, Mat image_spectrum, int noise_stddev){
    
	Mat padded = padd_image(I);
	Mat noise = rand_noise(padded, noise_stddev);
	Mat noise_spectrum = get_spectrum(noise);
    
	Scalar padded_mean = mean(padded);
    
	Mat planes[2];
	Mat complexI = get_dft(padded);
	split(complexI, planes);	// planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    
	Mat factor = image_spectrum / (image_spectrum + noise_spectrum);
	multiply(planes[0],factor,planes[0]);
	multiply(planes[1],factor,planes[1]);
    
    
	merge(planes, 2, complexI);
	idft(complexI, complexI);
	split(complexI, planes);
    //	normalize(planes[0], planes[0], 0, 128, CV_MINMAX );
	Scalar enhanced_mean = mean(planes[0]);
	double norm_factor =  padded_mean.val[0] / enhanced_mean.val[0];
	multiply(planes[0],norm_factor, planes[0]);
	Mat normalized;
	planes[0].convertTo(normalized, CV_8UC1);
	return normalized;
}

Mat padd_image(Mat I){
	Mat padded;
	int m = getOptimalDFTSize( I.rows );
	int n = getOptimalDFTSize( I.cols ); // on the border add zero pixels
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
	return padded;
}

Mat get_spectrum(Mat I){
	Mat complexI = get_dft(I);
	Mat planes[2];
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	Mat magI = planes[0];
	multiply(magI,magI,magI);
	return magI;
}

Mat get_dft(Mat I){
	Mat image;
	I.convertTo(image, CV_32F);
	Mat planes[] = {Mat_<float>(image), Mat::zeros(image.size(), CV_32F)};
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI);
	return complexI;
}

Mat with_noise(Mat image, int stddev){
	Mat noise(image.rows, image.cols, CV_8U);
	rand_noise(image, stddev).convertTo(noise, CV_8U);
	Mat noisy = image.clone();
	noisy += noise;
	return noisy;
}

Mat rand_noise(Mat I, int stddev){
	Mat noise = Mat::zeros(I.rows, I.cols, CV_32F);
	randn(noise,Scalar::all(0), Scalar::all(stddev));
	return noise;
}