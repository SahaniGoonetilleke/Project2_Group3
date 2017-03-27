#pragma once


#include <Windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <msclr/marshal_cppstd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include "spline.h"
#include "math.h"

cv::Mat manImage, monkeyImage, morphedImage;
cv::Mat editedImage;

namespace MyGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	protected:
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  importToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::PictureBox^  ManPictureBox;
	private: System::Windows::Forms::PictureBox^  MonkeyPictureBox;
	private: System::Windows::Forms::PictureBox^  MorphedPictureBox;



	private: System::Windows::Forms::TrackBar^  MorphedTrackBar;
	private: System::Windows::Forms::Button^  buttonManOpen;
	private: System::Windows::Forms::Button^  buttonMonkeyOpen;
	private: System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->importToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ManPictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->MonkeyPictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->MorphedPictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->MorphedTrackBar = (gcnew System::Windows::Forms::TrackBar());
			this->buttonManOpen = (gcnew System::Windows::Forms::Button());
			this->buttonMonkeyOpen = (gcnew System::Windows::Forms::Button());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ManPictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MonkeyPictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MorphedPictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MorphedTrackBar))->BeginInit();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->BackColor = System::Drawing::Color::SkyBlue;
			this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Padding = System::Windows::Forms::Padding(8, 2, 0, 2);
			this->menuStrip1->Size = System::Drawing::Size(1325, 28);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->importToolStripMenuItem,
					this->saveToolStripMenuItem, this->exitToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(44, 24);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// importToolStripMenuItem
			// 
			this->importToolStripMenuItem->Name = L"importToolStripMenuItem";
			this->importToolStripMenuItem->Size = System::Drawing::Size(135, 26);
			this->importToolStripMenuItem->Text = L"Import";
			this->importToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::importToolStripMenuItem_Click);
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(135, 26);
			this->saveToolStripMenuItem->Text = L"Save As";
			this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveToolStripMenuItem_click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(135, 26);
			this->exitToolStripMenuItem->Text = L"Exit";
			// 
			// ManPictureBox
			// 
			this->ManPictureBox->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->ManPictureBox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->ManPictureBox->Cursor = System::Windows::Forms::Cursors::Default;
			this->ManPictureBox->Location = System::Drawing::Point(16, 61);
			this->ManPictureBox->Margin = System::Windows::Forms::Padding(4);
			this->ManPictureBox->Name = L"ManPictureBox";
			this->ManPictureBox->Size = System::Drawing::Size(370, 400);
			this->ManPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->ManPictureBox->TabIndex = 1;
			this->ManPictureBox->TabStop = false;
			this->ManPictureBox->WaitOnLoad = true;
			this->ManPictureBox->Click += gcnew System::EventHandler(this, &MyForm::ManPicturebox_Click);
			this->ManPictureBox->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::ManPicturebox_MouseClick);
			this->ManPictureBox->MouseHover += gcnew System::EventHandler(this, &MyForm::ManPictureBox_MouseHover);
			// 
			// MonkeyPictureBox
			// 
			this->MonkeyPictureBox->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->MonkeyPictureBox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->MonkeyPictureBox->Location = System::Drawing::Point(394, 61);
			this->MonkeyPictureBox->Margin = System::Windows::Forms::Padding(4);
			this->MonkeyPictureBox->Name = L"MonkeyPictureBox";
			this->MonkeyPictureBox->Size = System::Drawing::Size(370, 400);
			this->MonkeyPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->MonkeyPictureBox->TabIndex = 12;
			this->MonkeyPictureBox->TabStop = false;
			this->MonkeyPictureBox->WaitOnLoad = true;
			this->MonkeyPictureBox->Click += gcnew System::EventHandler(this, &MyForm::MonkeyPictureBox_Click);
			this->MonkeyPictureBox->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::MonkeyPictureBox_MouseClick);
			this->MonkeyPictureBox->MouseHover += gcnew System::EventHandler(this, &MyForm::MonkeyPictureBox_MouseHover);
			// 
			// MorphedPictureBox
			// 
			this->MorphedPictureBox->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->MorphedPictureBox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->MorphedPictureBox->Location = System::Drawing::Point(772, 61);
			this->MorphedPictureBox->Margin = System::Windows::Forms::Padding(4);
			this->MorphedPictureBox->Name = L"MorphedPictureBox";
			this->MorphedPictureBox->Size = System::Drawing::Size(370, 400);
			this->MorphedPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->MorphedPictureBox->TabIndex = 13;
			this->MorphedPictureBox->TabStop = false;
			this->MorphedPictureBox->Click += gcnew System::EventHandler(this, &MyForm::MorphedPictureBox_Click);
			// 
			// MorphedTrackBar
			// 
			this->MorphedTrackBar->BackColor = System::Drawing::SystemColors::InfoText;
			this->MorphedTrackBar->Location = System::Drawing::Point(772, 487);
			this->MorphedTrackBar->Margin = System::Windows::Forms::Padding(4);
			this->MorphedTrackBar->Maximum = 100;
			this->MorphedTrackBar->Name = L"MorphedTrackBar";
			this->MorphedTrackBar->Size = System::Drawing::Size(370, 56);
			this->MorphedTrackBar->TabIndex = 14;
			this->MorphedTrackBar->Value = 50;
			this->MorphedTrackBar->Scroll += gcnew System::EventHandler(this, &MyForm::MorphedTrackBar_Scroll);
			// 
			// buttonManOpen
			// 
			this->buttonManOpen->Location = System::Drawing::Point(16, 468);
			this->buttonManOpen->Name = L"buttonManOpen";
			this->buttonManOpen->Size = System::Drawing::Size(138, 42);
			this->buttonManOpen->TabIndex = 15;
			this->buttonManOpen->Text = L"Import Man";
			this->buttonManOpen->UseVisualStyleBackColor = true;
			this->buttonManOpen->Click += gcnew System::EventHandler(this, &MyForm::ManOpen_click);
			// 
			// buttonMonkeyOpen
			// 
			this->buttonMonkeyOpen->Location = System::Drawing::Point(394, 468);
			this->buttonMonkeyOpen->Name = L"buttonMonkeyOpen";
			this->buttonMonkeyOpen->Size = System::Drawing::Size(144, 42);
			this->buttonMonkeyOpen->TabIndex = 16;
			this->buttonMonkeyOpen->Text = L"Import Monkey";
			this->buttonMonkeyOpen->UseVisualStyleBackColor = true;
			this->buttonMonkeyOpen->Click += gcnew System::EventHandler(this, &MyForm::MonkeyOpen_click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1325, 558);
			this->Controls->Add(this->buttonMonkeyOpen);
			this->Controls->Add(this->buttonManOpen);
			this->Controls->Add(this->MorphedTrackBar);
			this->Controls->Add(this->MorphedPictureBox);
			this->Controls->Add(this->MonkeyPictureBox);
			this->Controls->Add(this->ManPictureBox);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ManPictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MonkeyPictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MorphedPictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->MorphedTrackBar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	//xxxxxxxxxxxxx
	private: System::Void importToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		// Displays an OpenFileDialog so the user can select an image file.  
		OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
		openFileDialog1->Filter = "All Graphics Types|*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff";
		openFileDialog1->Title = "Select an Image File of a Man";
		// Show the Dialog.  
		// If the user clicked OK in the dialog and  
		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			// Assign the cursor in the Stream to  
			// the Form's Cursor property. 
			// cache.jpg is the copy of the original image used for editing
			ManPictureBox->Load(openFileDialog1->FileName);
			ManPictureBox->Image->Save("cacheMan.jpg", System::Drawing::Imaging::ImageFormat::Jpeg);
			editedImage = cv::imread("cacheMan.jpg", cv::IMREAD_COLOR);
		}
	}
	// Import and open image of Man
	private: System::Void ManOpen_click(System::Object^  sender, System::EventArgs^  e) {
		// Displays an OpenFileDialog so the user can select an image file.  
		OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
		openFileDialog1->Filter = "All Graphics Types|*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff";
		openFileDialog1->Title = "Select an Image File of a Man";
		// Show the Dialog.  
		// If the user clicked OK in the dialog and  
		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			// Assign the cursor in the Stream to  
			// the Form's Cursor property. 
			// cache.jpg is the copy of the original image used for editing
			ManPictureBox->Load(openFileDialog1->FileName);
			ManPictureBox->Image->Save("cacheMan.jpg", System::Drawing::Imaging::ImageFormat::Jpeg);
			manImage = cv::imread("cacheMan.jpg", cv::IMREAD_COLOR);
		}
	}
	// Import and open image of Monkey
	private: System::Void MonkeyOpen_click(System::Object^  sender, System::EventArgs^  e) {
		// Displays an OpenFileDialog so the user can select an image file.  
		OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
		openFileDialog1->Filter = "All Graphics Types|*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff";
		openFileDialog1->Title = "Select an Image File of a Monkey";
		// Show the Dialog.  
		// If the user clicked OK in the dialog and  
		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			// Assign the cursor in the Stream to  
			// the Form's Cursor property. 
			// cache.jpg is the copy of the original image used for editing
			MonkeyPictureBox->Load(openFileDialog1->FileName);
			MonkeyPictureBox->Image->Save("cacheMonkey.jpg", System::Drawing::Imaging::ImageFormat::Jpeg);
			monkeyImage = cv::imread("cacheMonkey.jpg", cv::IMREAD_COLOR);
			cv::resize(monkeyImage, monkeyImage, manImage.size()); // Resize monkey to be the same size as man

			//Read points
			std::vector<cv::Point2f> points1 = readPoints("Man.txt");
			std::vector<cv::Point2f> points2 = readPoints("Monkey.txt");
		}
	}
	// Save morphed image
	private: System::Void saveToolStripMenuItem_click(System::Object^  sender, System::EventArgs^  e) {
		char  savePath[100];
		// Displays an saveFileDialog so the user can enter an image file.  
		SaveFileDialog ^ saveFileDialog1 = gcnew SaveFileDialog();
		saveFileDialog1->Filter = "All Graphics Types|*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff";
		saveFileDialog1->Title = "Save As Morphed Image";
		saveFileDialog1->ShowDialog();
		// Show the Dialog.  
		if (saveFileDialog1->FileName)
		{
			std::string filePath = msclr::interop::marshal_as<std::string>(saveFileDialog1->FileName);
			sprintf(savePath, "%s", filePath.c_str());
			cv::imwrite(savePath, morphedImage);
		}
	}

	private: System::Void ManPicturebox_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		std::vector<cv::Point>* ptPtr = (std::vector<cv::Point>*)param;
		ptPtr->push_back(cv::Point(x, y));
		printf("%d %d\n", x, y);
	}
	private: System::Void MonkeyPictureBox_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
	}

	private: System::Void ManPicturebox_Click(System::Object^  sender, System::EventArgs^  e) {
		//cv::setMouseCallback(, mouseClick, (void*)&points);

	}
	private: System::Void MonkeyPictureBox_Click(System::Object^  sender, System::EventArgs^  e) {
	
	}
	private: System::Void MorphedPictureBox_Click(System::Object^  sender, System::EventArgs^  e) {
	}

	private: System::Void MorphedTrackBar_Scroll(System::Object^  sender, System::EventArgs^  e) {
		int trackBarValueMorphed = MorphedTrackBar->Value;		// Acquire track bar value
		cv::Mat morphedImage;									// Initiate a Mat to save the cacheMorphed.jpg image
		//morphedImage = manImage.clone();
		Blend(&manImage, &monkeyImage, trackBarValueMorphed);	//Call Blend function
	}

			 // Functions defined in opencv

			 // Store mouse click co-ordinates in txt file
			 void mouseClick(int evt, int x, int y, int flags, void* param) {
				 if (evt == CV_EVENT_LBUTTONDOWN) {
					 std::vector<cv::Point>* ptPtr = (std::vector<cv::Point>*)param;
					 ptPtr->push_back(cv::Point(x, y));
					 printf("%d %d\n", x, y);
				 }
			 }

			 // Read points from text file
			 std::vector<cv::Point2f> readPoints(std::string pointsFileName)
			 {
				 std::vector<cv::Point2f> points;
				 std::ifstream ifs(pointsFileName);
				 float x, y;
				 while (ifs >> x >> y)
				 {
					 points.push_back(cv::Point2f(x, y));
				 }
				 return points;
			 }

			 // Feature matching - local warp
			 void Warping(cv::Mat &img1, cv::Mat &img2, cv::Mat &img, std::vector<cv::Point2f> &t1, std::vector<cv::Point2f> &t2, std::vector<cv::Point2f> &t, double alpha)
			 {
				 // Find bounding rectangle for each triangle
				 cv::Rect r = cv::boundingRect(t);
				 cv::Rect r1 = cv::boundingRect(t1);
				 cv::Rect r2 = cv::boundingRect(t2);

				 // Offset points by left top corner of the respective rectangles
				 std::vector<cv::Point2f> t1Rect, t2Rect, tRect;
				 std::vector<cv::Point> tRectInt;
				 for (int i = 0; i < 3; i++)
				 {
					 tRect.push_back(cv::Point2f(t[i].x - r.x, t[i].y - r.y));
					 tRectInt.push_back(cv::Point(t[i].x - r.x, t[i].y - r.y)); // for fillConvexPoly

					 t1Rect.push_back(cv::Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
					 t2Rect.push_back(cv::Point2f(t2[i].x - r2.x, t2[i].y - r2.y));
				 }

				 // Get mask by filling triangle
				 cv::Mat mask = cv::Mat::zeros(r.height, r.width, CV_32FC3);
				 cv::fillConvexPoly(mask, tRectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

				 // Apply warpImage to small rectangular patches
				 cv::Mat img1Rect, img2Rect;
				 img1(r1).copyTo(img1Rect);
				 img2(r2).copyTo(img2Rect);

				 cv::Mat warpImage1 = cv::Mat::zeros(r.height, r.width, img1Rect.type());
				 cv::Mat warpImage2 = cv::Mat::zeros(r.height, r.width, img2Rect.type());

				 applyAffineTransform(warpImage1, img1Rect, t1Rect, tRect);
				 applyAffineTransform(warpImage2, img2Rect, t2Rect, tRect);

			 }



			 // Affine Transform
			 void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri, std::vector<cv::Point2f> &dstTri)
			 {
				 // Given a pair of triangles, find the affine transform.
				 cv::Mat warpMat = cv::getAffineTransform(srcTri, dstTri);

				 // Apply the Affine Transform just found to the src image
				 cv::warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
			 }


			 // Global Warp -- Not essential ;)
			 void Alignment()
			 {

			 }


			 // Linear blending/ Cross-dissolve function
			 void Blend(cv::Mat *manImage, cv::Mat *monkeyImage, int tempValueMorphed)
			 {
				 cv::Mat man = *manImage;
				 cv::Mat monkey = *monkeyImage;
				 cv::resize(monkey, monkey, man.size()); // Resize monkey to be the same size as man
				 double alpha = tempValueMorphed / 100.0;
				 double beta = (1.0 - alpha);
				 cv::addWeighted(monkey, alpha, man, beta, 0.0, morphedImage);
				 refreshMorphedPicBox(morphedImage);
			 }

			 // Display image  ---------xxxxxxxxxxxxxxx
			 int displayImage(cv::Mat *mainImage, cv::String imageDes)
			 {
				 cv::Mat tempImage;
				 tempImage = *mainImage;
				 if (tempImage.empty())                      // Check for invalid input
				 {
					 return -1;
				 }
				 namedWindow(imageDes, cv::WINDOW_AUTOSIZE);		// Create a window for display.
				 imshow(imageDes, tempImage);                // Show our image inside it.
				 cv::waitKey(0);									// Wait for a keystroke in the window
				 return 0;
			 }
			 
			 // Show updated Morphed image
			 int refreshMorphedPicBox(cv::Mat displayImage)
			 {
				 System::Drawing::Graphics^ graphics = MorphedPictureBox->CreateGraphics();
				 System::IntPtr ptr(displayImage.ptr());
				 System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(displayImage.cols, displayImage.rows, displayImage.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, ptr);
				 System::Drawing::RectangleF rect(0, 0, MorphedPictureBox->Width, MorphedPictureBox->Height);
				 graphics->DrawImage(b, rect);
				 return 0;
			 }




private: System::Void ManPictureBox_MouseHover(System::Object^  sender, System::EventArgs^  e) {
}
private: System::Void MonkeyPictureBox_MouseHover(System::Object^  sender, System::EventArgs^  e) {
}

};
}