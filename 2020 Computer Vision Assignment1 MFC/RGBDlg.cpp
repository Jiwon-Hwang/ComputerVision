
// RGBDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "RGB.h"
#include "RGBDlg.h"
#include "afxdialogex.h"
#include <iostream>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") //콘솔창 출력용 --> 삭제하기

#ifdef _DEBUG
#define new DEBUG_NEW

#define CALL_LabelComponent(x,y,returnLabel) { STACK[SP] = x; STACK[SP + 1] = y; STACK[SP + 2] = returnLabel; SP += 3; goto START; }
#define RETURN { SP -= 3;                \
                 switch (STACK[SP+2])    \
                 {                       \
                 case 1 : goto RETURN1;  \
                 case 2 : goto RETURN2;  \
                 case 3 : goto RETURN3;  \
                 case 4 : goto RETURN4;  \
                 default: return;        \
                 }                       \
               }
#define X (STACK[SP-3])
#define Y (STACK[SP-2])


#define FOREWARD 1
#define BACKWARD 2

#endif
void changeColor(Mat img, Mat &copy, int i);
void cvtToGray(Mat img, Mat& img_gray, int nRows, int nCols);
void Otsu(Mat& img_copy, int nRows, int nCols);
void Erode(Mat& src, Mat& dst, Mat& kernel, int nRows, int nCols);
void Dilate(Mat& src, Mat& dst, Mat& kernel, int nRows, int nCols);
void LabelComponent(unsigned short* STACK, unsigned short width, unsigned short height,
	Mat& input, Mat& output, int labelNo, unsigned short x, unsigned short y);
void LabelImage(unsigned short width, unsigned short height, Mat& input, Mat& output);
void ContourTracing(Mat &imgSrc, int sx, int sy, vector<Point>& cp, int w, int h);

void read_neighbor8(int y, int x, int neighbor8[], Mat& bImage);
void calCoord(int i, int* y, int* x);
void LabelingwithBT(Mat &bImage);
void BTracing8(int y, int x, int label, int tag, Mat& bImage, int** labImage);


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRGBDlg 대화 상자



CRGBDlg::CRGBDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RGB_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRGBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Img, m_pic);
}

BEGIN_MESSAGE_MAP(CRGBDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Img_Search, &CRGBDlg::OnBnClickedImgSearch)
	ON_BN_CLICKED(IDC_Img_Save, &CRGBDlg::OnBnClickedImgSave)	
	/* 원본 버튼 코드들
	ON_BN_CLICKED(IDC_Red_Btn, &CRGBDlg::OnBnClickedRedBtn)
	ON_BN_CLICKED(IDC_Green_Btn, &CRGBDlg::OnBnClickedGreenBtn)
	ON_BN_CLICKED(IDC_Blue_Btn, &CRGBDlg::OnBnClickedBlueBtn)
	ON_BN_CLICKED(IDC_Gray_Btn, &CRGBDlg::OnBnClickedGrayBtn)
	ON_BN_CLICKED(IDC_Btn_Otsu, &CRGBDlg::OnBnClickedBtnOtsu)
	*/
END_MESSAGE_MAP()


// CRGBDlg 메시지 처리기

BOOL CRGBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRGBDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRGBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRGBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// Load 버튼 클릭 시
void CRGBDlg::OnBnClickedImgSearch()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		pathName = dlg.GetPathName();
		CT2CA pszConvertedAnsiString_up(pathName);
		std::string up_pathName_str(pszConvertedAnsiString_up);
		img = cv::imread(up_pathName_str);
		DisplayImage(img, 3);

	}
}
void CRGBDlg::DisplayImage(Mat targetMat, int channel)
{
	CDC *pDC = nullptr;
	CImage* mfcImg = nullptr;

	pDC = m_pic.GetDC();
	CStatic *staticSize = (CStatic *)GetDlgItem(IDC_Img);
	staticSize->GetClientRect(rect);

	cv::Mat tempImage;
	cv::resize(targetMat, tempImage, Size(rect.Width(), rect.Height()));

	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biWidth = tempImage.cols;
	bitmapInfo.bmiHeader.biHeight = tempImage.rows;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biClrImportant = 0;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;

	if (targetMat.channels() == 3)
	{
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 24);
	}
	else if (targetMat.channels() == 1)
	{
		cvtColor(tempImage, tempImage, CV_GRAY2RGB);
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 24);
	}
	else if (targetMat.channels() == 4)
	{
		bitmapInfo.bmiHeader.biBitCount = 32;
		mfcImg = new CImage();
		mfcImg->Create(tempImage.cols, tempImage.rows, 32);
	}
	cv::flip(tempImage, tempImage, 0);
	::StretchDIBits(mfcImg->GetDC(), 0, 0, tempImage.cols, tempImage.rows,
		0, 0, tempImage.cols, tempImage.rows, tempImage.data, &bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

	mfcImg->BitBlt(::GetDC(m_pic.m_hWnd), 0, 0);

	if (mfcImg)
	{
		mfcImg->ReleaseDC();
		delete mfcImg;// mfcImg = nullptr;
	}
	tempImage.release();
	ReleaseDC(pDC);
}

// save 버튼 클릭 시, 색변환 & 저장하는 부분 / // 색 변경 => (X) Mat img_copy_r, img_copy_g, img_copy_b, img_copy_rg, img_copy_gg, img_copy_bg, img_copy_rgo, img_copy_ggo, img_copy_bgo = img.clone();
void CRGBDlg::OnBnClickedImgSave()
{
	
	//중간 과제

	//명암 영상 생성
	int nRows = img.rows;
	int nCols = img.cols;
	Mat img_copy(nRows, nCols, CV_8UC1); // Mat img(rows, cols, type)
	cvtToGray(img, img_copy, nRows, nCols); // cvtColor(img, img_copy, CV_BGR2GRAY);
	imwrite("img_gray.jpg", img_copy); 

	//이진화
	Otsu(img_copy, nRows, nCols);
	imwrite("img_Otsu.jpg", img_copy);

	//형태학적 연산 (morphologyEx)
	Mat kernel(5, 5, CV_8U, Scalar(1)); // Mat kernel = Mat::ones(5, 5, CV_8U);

	//Opening (열림연산) : erode(침식) -> dilate(팽창)
	Mat img_tmp1 = img_copy.clone();
	Mat img_opening = img_copy.clone();
	Erode(img_copy, img_tmp1, kernel, nRows, nCols);
	Dilate(img_tmp1, img_opening, kernel, nRows, nCols);
	imwrite("img_opening5.jpg", img_opening);

	//Closing (닫힘연산) : dilate(팽창) -> erode(침식)
	Mat img_tmp2 = img_copy.clone();
	Mat img_closing = img_copy.clone();
	Dilate(img_copy, img_tmp2, kernel, nRows, nCols);
	Erode(img_tmp2, img_closing, kernel, nRows, nCols);
	imwrite("img_closing5.jpg", img_closing);
	
	//Contour Tracing (레이블링)
	vector<vector<Point>> contours1;
	vector<Vec4i> hierarchy1;
	findContours(img_opening, contours1, hierarchy1, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat img_opening_ct(nRows, nCols, CV_8U, Scalar(255));
	img_opening_ct.setTo(cv::Scalar(255));
	drawContours(img_opening_ct, contours1,
		-1,    // 모든 외곽선 그리기
		Scalar(0), // 검게
		2);    // 두께를 2로

	imwrite("img_opening_ct.jpg", img_opening_ct);

	vector<vector<Point>> contours2;
	vector<Vec4i> hierarchy2;
	findContours(img_closing, contours2, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat img_closing_ct(nRows, nCols, CV_8U, Scalar(255));
	img_closing_ct.setTo(cv::Scalar(255));
	drawContours(img_closing_ct, contours2,
		-1,    // 모든 외곽선 그리기
		Scalar(0), // 검게
		2);    // 두께를 2로

	imwrite("img_closing_ct.jpg", img_closing_ct);

	
	
	/*
	// 기존 ContourTracing 함수
	vector<Point> cp;
	ContourTracing(img_opening, 0, 0, cp, nCols, nRows);
	imwrite("img_opening_ct.jpg", img_opening);
	ContourTracing(img_closing, 0, 0, cp, nCols, nRows);
	imwrite("img_closing_ct.jpg", img_closing);
	*/
	

	//// 새로운 contour tracing 함수
	//LabelingwithBT(img_opening);
	//imwrite("img_opening_ct.jpg", img_opening);
	//LabelingwithBT(img_closing);
	//imwrite("img_closing_ct.jpg", img_closing);
	

	/* 새로운 ct 함수 2
	Mat img_opening_ct(nRows, nCols, CV_8U, Scalar(255));
	LabelImage(nCols, nRows, img_opening, img_opening_ct);
	imwrite("img_opening_ct.jpg", img_opening_ct);
	*/


	/*
	//과제2 (RGB & GRAY & Otsu)

	//1. changeColor & imwrite
	//red
	Mat img_copy_r = img.clone(); //Mat img_copy_r; 이렇게 선언만 하는 건 왜 안됨.. cvtColor에서는 선언만 하는거 가능한데.. &주소 때문..?
	changeColor(img, img_copy_r, 1);
	imwrite("copy_r.jpg", img_copy_r);

	//green
	Mat img_copy_g = img.clone();
	changeColor(img, img_copy_g, 2);
	imwrite("copy_g.jpg", img_copy_g);

	//blue
	Mat img_copy_b = img.clone();
	changeColor(img, img_copy_b, 3);
	imwrite("copy_b.jpg", img_copy_b);


	//red->gray
	Mat img_copy_rg; // 선언만 해줘도 됨
	cvtColor(img_copy_r, img_copy_rg, CV_BGR2GRAY); //cvtColor(원본 이미지(입력), 변환된 이미지(출력), 컬러 변환 코드)
	imwrite("copy_rg.jpg", img_copy_rg);

	//green->gray
	Mat img_copy_gg;
	cvtColor(img_copy_g, img_copy_gg, CV_BGR2GRAY); 
	imwrite("copy_gg.jpg", img_copy_gg);

	//blue->gray
	Mat img_copy_bg;
	cvtColor(img_copy_b, img_copy_bg, CV_BGR2GRAY);
	imwrite("copy_bg.jpg", img_copy_bg);


	//red->gray->Otsu
	Mat img_copy_rgo = img_copy_rg.clone();
	Otsu(img_copy_rgo); //Otsu(&img)
	imwrite("copy_rgo.jpg", img_copy_rgo);

	//green->gray->Otsu
	Mat img_copy_ggo = img_copy_gg.clone();
	Otsu(img_copy_ggo); 
	imwrite("copy_ggo.jpg", img_copy_ggo);
	
	//blue->gray->Otsu
	Mat img_copy_bgo = img_copy_bg.clone();
	Otsu(img_copy_bgo);
	imwrite("copy_bgo.jpg", img_copy_bgo);
	


	//과제3 (opening & closing 연산)

	// 필터 효과를 더 두드러지게 5x5 구조 요소를 사용
	Mat element5(5, 5, CV_8U, Scalar(1));
	
	//Opening (열림연산) : erode(침식) -> dilate(팽창)
	Mat rgo_opening;    //여기서 왜 clone은 안해줘도 되는지는...?
	morphologyEx(img_copy_rgo, rgo_opening, MORPH_OPEN, element5);
	//imwrite("rgo_opening.jpg", rgo_opening);

	Mat ggo_opening;
	morphologyEx(img_copy_ggo, ggo_opening, MORPH_OPEN, element5);
	//imwrite("ggo_opening.jpg", ggo_opening);

	Mat bgo_opening;
	morphologyEx(img_copy_bgo, bgo_opening, MORPH_OPEN, element5);
	//imwrite("bgo_opening.jpg", bgo_opening);


	//Closing (닫힘연산) : dilate(팽창) -> erode(침식)
	Mat rgo_closing;    //여기서 왜 clone은 안해줘도 되는지는...?
	morphologyEx(img_copy_rgo, rgo_closing, MORPH_CLOSE, element5);
	//imwrite("rgo_closing.jpg", rgo_closing);

	Mat ggo_closing;
	morphologyEx(img_copy_ggo, ggo_closing, MORPH_CLOSE, element5);
	//imwrite("ggo_closing.jpg", ggo_closing);

	Mat bgo_closing;
	morphologyEx(img_copy_bgo, bgo_closing, MORPH_CLOSE, element5);
	//imwrite("bgo_closing.jpg", bgo_closing);
	*/

	//2. imshow
	/*
	imshow("Red", img_copy_r);
	imshow("Green", img_copy_g);
	imshow("Blue", img_copy_b);
	imshow("Red_Gray", img_copy_rg);
	imshow("Green_Gray", img_copy_gg);
	imshow("Blue_Gray", img_copy_bg);
	*/
	/*
	imshow("rgo", img_copy_rgo);
	imshow("ggo", img_copy_ggo);
	imshow("bgo", img_copy_bgo);
	

	imshow("rgo_opening", rgo_opening);
	imshow("ggo_opening", ggo_opening);
	imshow("bgo_opening", bgo_opening);
	imshow("rgo_closing", rgo_closing);
	imshow("ggo_closing", ggo_closing);
	imshow("bgo_closing", bgo_closing);
	*/
	
	/*
	//과제4 (contour tracing)
	vector<Point> cp;

	ContourTracing(rgo_opening, 0, 0, cp);
	ContourTracing(ggo_opening, 0, 0, cp);
	ContourTracing(bgo_opening, 0, 0, cp);
	ContourTracing(rgo_closing, 0, 0, cp);
	ContourTracing(ggo_closing, 0, 0, cp);
	ContourTracing(bgo_closing, 0, 0, cp);

	imwrite("rgo_opening_ct.jpg", rgo_opening);
	imwrite("ggo_opening_ct.jpg", ggo_opening);
	imwrite("bgo_opening_ct.jpg", bgo_opening);
	imwrite("rgo_closing_ct.jpg", rgo_closing);
	imwrite("ggo_closing_ct.jpg", ggo_closing);
	imwrite("bgo_closing_ct.jpg", bgo_closing);

	imshow("rgo_opening_ct", rgo_opening);
	imshow("ggo_opening_ct", ggo_opening);
	imshow("bgo_opening_ct", bgo_opening);
	imshow("rgo_closing_ct", rgo_closing);
	imshow("ggo_closing_ct", ggo_closing);
	imshow("bgo_closing_ct", bgo_closing);

	waitKey(0);
	destroyAllWindows();
	*/


	/* 원래 코드
	imwrite("copy.jpg", img);
	MessageBox(_T("이미지 저장 완료!"), _T(""));
	//imshow();
	*/
}

/* 원래 있던 버튼들 코드
void CRGBDlg::OnBnClickedRedBtn()
{
	Mat img_copy = img.clone();
	changeColor(img, img_copy, 1);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}


void CRGBDlg::OnBnClickedGreenBtn()
{
	Mat img_copy = img.clone();
	changeColor(img, img_copy, 2);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}


void CRGBDlg::OnBnClickedBlueBtn()
{
	Mat img_copy = img.clone();
	changeColor(img, img_copy, 3);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}

void CRGBDlg::OnBnClickedGrayBtn()
{
	Mat img_copy = img.clone();
	cvtColor(img_copy, img_copy, CV_BGR2GRAY);
	DisplayImage(img_copy, 3);
	img = img_copy.clone();
}


void CRGBDlg::OnBnClickedBtnOtsu()
{
	Mat img_copy = img.clone();
	int nRows = img_copy.rows;
	int nCols = img_copy.cols;
	Mat histogram;
	const int* channel_numbers = { 0 };
	float channel_range[] = { 0.0, 255.0 };
	const float* channel_ranges = channel_range;
	int number_bins = 255;
	calcHist(&img_copy, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);
	float * n = histogram.ptr<float>(0);
	n[255] = 0.0;

	double p[256] = { 0.0 };
	double w0[256] = { 0.0 };
	double w1[256] = { 0.0 };
	double m0[256] = { 0.0 };
	double m1[256] = { 0.0 };
	double sum_ipi[256] = { 0.0 };
	double mt = 0.0;
	double sq = 0;
	double sol_sq = 0.0;
	int T = 0;

	for (int i = 1; i <= 255; i++) {
		p[i] = n[i] / 262144;
		mt += (i*p[i]);
		w0[i] = w0[i - 1] + p[i];
		w1[i] = 1 - w0[i];
		sum_ipi[i] = sum_ipi[i - 1] + (i*p[i]);
	}

	for (int i = 1; i <= 255; i++) {
		m0[i] = (sum_ipi[i] / w0[i]);
		m1[i] = ((sum_ipi[255] - sum_ipi[i]) / w1[i]);
	}

	for (int i = 1; i <= 255; i++) {
		sq = (w0[i])*(1 - w0[i])*((m0[i] - m1[i])*(m0[i] - m1[i]));
		if (sq > sol_sq) {
			sol_sq = sq;
			T = i;
		}
	}

	Mat binary;
	threshold(img_copy, binary, T, 255, CV_THRESH_BINARY);
	DisplayImage(binary, 3);
	img = binary.clone();
}
*/

void changeColor(Mat img, Mat &copy, int i)
{
	if (i == 1)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x<copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = ptr1[3 * x + 2];
			}
		}
	}
	else if (i == 2)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x<copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				ptr1[3 * x + 0] = 0;
				ptr1[3 * x + 1] = ptr1[3 * x + 1];
				ptr1[3 * x + 2] = 0;
			}
		}
	}
	else if(i == 3)
	{
		for (int y = 0; y < copy.rows; y++)
		{
			unsigned char* ptr1 = copy.data + 3 * (copy.cols*y);
			unsigned char* resultptr = copy.data + 3 * (copy.cols*y);
			for (int x = 0; x<copy.cols; x++)
			{
				// 이렇게 RGB값을 조정하여 그 범위 안에 있는 Rgb 픽셀값에 단색을 넣었다.
				//200 -> 160 -> 110
				//그림자
				//배경색을 초록으로 해도 결과가 바뀌므로 함부로 손대지 말자
				ptr1[3 * x + 0] = ptr1[3 * x + 0];
				ptr1[3 * x + 1] = 0;
				ptr1[3 * x + 2] = 0;
			}
		}
	}

}

void cvtToGray(Mat img, Mat &img_gray, int nRows, int nCols) {
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {
			uchar b = img.at<Vec3b>(i, j)[0]; // 접근할 픽셀의 (y, x)좌표값을 통해 이미지의 픽셀값에 접근
			uchar g = img.at<Vec3b>(i, j)[1];
			uchar r = img.at<Vec3b>(i, j)[2];

			img_gray.at<uchar>(i, j) = (r + g + b) / 3.0;
		}
	}
}


void Otsu(Mat &img_copy, int nRows, int nCols)
{
	Mat histogram;
	const int* channel_numbers = { 0 };
	float channel_range[] = { 0.0, 255.0 };
	const float* channel_ranges = channel_range;
	int number_bins = 255;
	calcHist(&img_copy, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);
	float* n = histogram.ptr<float>(0);
	n[255] = 0.0;

	double p[256] = { 0.0 };
	double w0[256] = { 0.0 };
	double w1[256] = { 0.0 };
	double m0[256] = { 0.0 };
	double m1[256] = { 0.0 };
	double sum_ipi[256] = { 0.0 };
	double mt = 0.0;
	double sq = 0;
	double sol_sq = 0.0;
	int T = 0;

	for (int i = 1; i <= 255; i++) {
		p[i] = n[i] / 262144;
		mt += (i * p[i]);
		w0[i] = w0[i - 1] + p[i];
		w1[i] = 1 - w0[i];
		sum_ipi[i] = sum_ipi[i - 1] + (i * p[i]);
	}

	for (int i = 1; i <= 255; i++) {
		m0[i] = (sum_ipi[i] / w0[i]);
		m1[i] = ((sum_ipi[255] - sum_ipi[i]) / w1[i]);
	}

	for (int i = 1; i <= 255; i++) {
		sq = (w0[i]) * (1 - w0[i]) * ((m0[i] - m1[i]) * (m0[i] - m1[i]));
		if (sq > sol_sq) {
			sol_sq = sq;
			T = i;
		}
	}

	Mat binary;
	//printf("%d", T); // T:이미지마다 다름
	threshold(img_copy, binary, T, 255, CV_THRESH_BINARY);
	//DisplayImage(binary, 3);
	img_copy = binary.clone();
}


// Erode : 필터 영역 내 픽셀들 중 최소 픽셀 값(0)을 현재 픽셀 값에 대입, 최소값(min)필터 역할
void Erode(Mat& src, Mat& dst, Mat& kernel, int nRows, int nCols) {
	int iMin, iVal;
	for (int i = 0; i < nRows - 4; i++) {
		for (int j = 0; j < nCols - 4; j++) {
			iMin = 0xFFF;
			for (int ii = 0; ii < kernel.rows; ii++) {
				for (int jj = 0; jj < kernel.cols; jj++) {
					if (kernel.at<uchar>(ii, jj)) {
						iVal = src.at<uchar>(i + ii, j + jj);
						if (iMin > iVal) {
							iMin = iVal;
						}
					}
				}
			}
			dst.at<uchar>(i + 1, j + 1) = iMin;
		}
	}
}

// Dilation : 필터 영역 내 픽셀들 중 최대 픽셀 값(255)을 현재 픽셀 값에 대입, 최대값(max)필터 역할
void Dilate(Mat& src, Mat& dst, Mat& kernel, int nRows, int nCols) {
	int iMax, iVal;
	for (int i = 0; i < nRows - 4; i++) {
		for (int j = 0; j < nCols - 4; j++) {
			iMax = 0x000;
			for (int ii = 0; ii < kernel.rows; ii++) {
				for (int jj = 0; jj < kernel.cols; jj++) {
					if (kernel.at<uchar>(ii, jj)) {
						iVal = src.at<uchar>(i + ii, j + jj);
						if (iMax < iVal) {
							iMax = iVal;
						}
					}
				}
			}
			dst.at<uchar>(i + 1, j + 1) = iMax;
		}
	}
}



void ContourTracing(Mat &imgSrc, int sx, int sy, vector<Point>& cp, int w, int h)
{
	// 외곽선 좌표를 저장할 구조체 초기화
	cp.clear();

	/* ***원래 객체 : 흰색(255) ==> 내 이미지는 객체(0)으로 두기! 검정부분! (전체 주석 아래 코드부터)
	// 외곽선 추적 시작 픽셀이 객체(255)가 아니면 종료
	if (imgSrc.at<uchar>(sy, sx) != 255)
		return;
	*/

	int x, y, nx, ny;
	int d, cnt;
	int  dir[8][2] = { // 진행 방향을 나타내는 배열
		{ 1,  0 },
		{ 1,  1 },
		{ 0,  1 },
		{ -1,  1 },
		{ -1,  0 },
		{ -1, -1 },
		{ 0, -1 },
		{ 1, -1 }
	};

	x = sx;
	y = sy;
	d = cnt = 0;

	while (1)
	{
		nx = x + dir[d][0];
		ny = y + dir[d][1];

		if (nx < 0 || nx >= w || ny < 0 || ny >= h || imgSrc.at<uchar>(ny, nx) == 0xfff)
		{
			// 진행 방향에 있는 픽셀이 객체(0)가 아닌 "배경"일 경우(255),
			// 시계 방향으로 진행 방향을 바꾸고 다시 시도한다.

			if (++d > 7) d = 0;
			cnt++;

			// 8방향 모두 배경(255)인 경우 
			if (cnt >= 8)
			{
				cp.push_back(Point(x, y));
				break;  // 외곽선 추적을 끝냄.
			}
		}
		else
		{
			// 진행 방향의 픽셀이 "객체"일 경우, 현재 점을 외곽선 정보에 저장
			cp.push_back(Point(x, y));

			// 진행 방향으로 이동
			x = nx;
			y = ny;

			// 방향 정보 초기화
			cnt = 0;
			d = (d + 6) % 8;	// d = d - 2 와 같은 형태
		}

		// 시작점으로 돌아왔고, 진행 방향이 초기화된 경우
		// 외곽선 추적을 끝낸다.
		if (x == sx && y == sy && d == 0) {
			imgSrc = Scalar(255); //Mat 객체(이미지) 255(흰색)으로 초기화

			int len = cp.size(); //cp.size() : vector 구조체에 저장된 외각선 point들 개수(구조체 크기) : ex.6050
			printf("%d\n", len);

			for (int i = 0; i < len; i++) {
				imgSrc.at<uchar>(cp[i].y, cp[i].x) = 0x000; //검정색으로 외각선 칠하기 (point들)
			}

			/*
			for (int i = 100; i<200; i++) {
				printf("%d %d %d\n", cp[i].x, cp[i].y, cp[i]); // 외곽선 좌표들 출력해보기
			}
			*/
			break;
		}
			
	}
}


void read_neighbor8(int y, int x, int neighbor8[], Mat &bImage) {
	//uchar b = img.at<Vec3b>(y, x)[0]; ==> 3채널 이미지의 픽셀값 가져오기
	//img.at<uchar>(y, x); ==> 1채널 이미지의 픽셀값 가져오기
	neighbor8[0] = bImage.at<uchar>(y, x + 1);   neighbor8[1] = bImage.at<uchar>(y + 1, x + 1);
	neighbor8[2] = bImage.at<uchar>(y + 1, x);   neighbor8[3] = bImage.at<uchar>(y + 1, x - 1);
	neighbor8[4] = bImage.at<uchar>(y, x - 1);   neighbor8[5] = bImage.at<uchar>(y - 1, x - 1);
	neighbor8[6] = bImage.at<uchar>(y - 1, x);   neighbor8[7] = bImage.at<uchar>(y - 1, x + 1);
} /* end of "Read_neighbor8" */



void calCoord(int i, int* y, int* x) {
	switch (i) {
	case 0:  *x = *x + 1;  break;
	case 1:  *y = *y + 1; *x = *x + 1; break;
	case 2:  *y = *y + 1; break;
	case 3:  *y = *y + 1; *x = *x - 1; break;
	case 4:  *x = *x - 1; break;
	case 5:  *y = *y - 1; *x = *x - 1; break;
	case 6:  *y = *y - 1; break;
	case 7:  *y = *y - 1; *x = *x + 1; break;
	}  /* end of "switch" */
}


// LUT 그림처럼 초기화 미리 해두기(가능한 case들)
int LUT_BLabeling8[8][8] =  
{
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 0, 0},
	{0, 0, 0, 0, 0, 1, 1, 0},
	{0, 0, 0, 0, 0, 1, 1, 1},
	{1, 0, 0, 0, 0, 1, 1, 1},
	{1, 1, 0, 0, 0, 1, 1, 1},
	{1, 1, 1, 0, 0, 1, 1, 1},
	{1, 1, 1, 1, 0, 1, 1, 1}
};


//int num_region[1000000];

void LabelingwithBT(Mat &bImage) {
	int WIDTH = bImage.cols;
	int HEIGHT = bImage.rows;
	//printf("%d\n", WIDTH); ==> 568
	//printf("%d\n", HEIGHT); ==> 568

	//int labImage[10000][10000] = { 0, }; // label 값들 ++하며 기록할 배열(3종류..?), [HEIGHT][WIDTH] 이렇게 변수로 크기 지정 불가!
	
	//메모리 동적할당
	int** labImage = new int* [10000];
	for (int i = 0; i < 10000; i++) {
		labImage[i] = new int[10000];
		memset(labImage[i], 0, sizeof(int) * 10000);//0으로 초기화 하는 부분
	}

	/*
	//영상 전체(?)를 초기화 (label을 초기화) ==> 3가지 종류의 영역 start, propagation, hole 각각 픽셀 몇개씩인지 기록하는거 아님..?
	for (int i = 0; i < MAX_SIZE; i++) { 
		num_region[i] = 0;
	}
	*/
	int labelnumber = 1;

	for (int i = 1; i < (HEIGHT - 1); i++) {
		for (int j = 1; j < (WIDTH - 1); j++) {
			int cur_p = bImage.at<uchar>(i, j); // 현재 위치 읽어오기
			//printf("%d\n", cur_p); //==> 모두 255.. 맨 마지막에만 0 (0 하나만 가져오고 끝남)
			if (cur_p == 0) {   // object
				int ref_p1 = labImage[i][j - 1]; // 내 앞에꺼
				int ref_p2 = labImage[i - 1][j - 1]; // 내 대각선 위에 꺼
				if (ref_p1 > 1) {   // propagation (내 앞에꺼가 일단 이미 값이 할당 되었으면(이미 레이블 할당됨), 나는 전파 조건에 따라 전파  
					//num_region[ref_p1]++;
					labImage[i][j] = ref_p1;
				}
				else  if ((ref_p1 == 0) && (ref_p2 >= 2)) {   // hole
					//num_region[ref_p2]++;
					labImage[i][j] = ref_p2;
					BTracing8(i, j, ref_p2, BACKWARD, bImage, labImage); // hole이니까 반대쪽으로 tracing 하도록
				}
				else  if ((ref_p1 == 0) && (ref_p2 == 0)) { // region start (시작점)
					labelnumber++;
					//num_region[labelnumber]++;
					labImage[i][j] = labelnumber;
					BTracing8(i, j, labelnumber, FOREWARD, bImage, labImage); // 시작점이니까 순방향으로 tracing
				}
			}
			//else labImage[i][j] = 0;   // background (원래 0으로 초기화 해놔도 됨)
		}
	}

	//메모리 해제
	for (int i = 0; i < 10000; i++) {
		delete[] labImage[i];
	}
	delete[] labImage;
}


void BTracing8(int y, int x, int label, int tag, Mat &bImage, int **labImage) {
	int cur_orient, pre_orient;
	int end_x, pre_x, end_y, pre_y;
	int start_o, add_o;

	if (tag == FOREWARD) cur_orient = pre_orient = 0;
	else cur_orient = pre_orient = 6;
	end_x = pre_x = x;
	end_y = pre_y = y;

	// do~while문
	do {
		int neighbor8[8];
		read_neighbor8(y, x, neighbor8, bImage); //neighbbor8는 배열이름이므로, 배열의 첫 주소 넘어감
		start_o = (8 + cur_orient - 2) % 8;
		for (int i = 0; i < 8; i++) {
			add_o = (start_o + i) % 8;        
			if (neighbor8[add_o] != 0) break;
		}

		calCoord(add_o, &y, &x);
		cur_orient = add_o; 
		

		if (LUT_BLabeling8[pre_orient][cur_orient]) {
			//num_region[label]++;
			labImage[pre_y][pre_x] = label;
		}

		pre_x = x;
		pre_y = y;
		pre_orient = cur_orient;
	} 
	while ((y != end_y) || (x != end_x));
}


void LabelComponent(unsigned short* STACK, unsigned short width, unsigned short height,
	Mat& input, Mat& output, int labelNo, unsigned short x, unsigned short y)
{
	printf("LabelComponent : %d %d\n", width, height);
	STACK[0] = x;
	STACK[1] = y;
	STACK[2] = 0;  /* return - component is labelled */
	int SP = 3;
	//int index;

START: /* Recursive routine starts here */

	//index = X + width * Y; // 2차원 배열의 인덱스를 1차원처럼 늘려서 계산한 것!
	if (input.at<uchar>(Y, X) == 255) RETURN;   /* This pixel is not part of a component */
	if (output.at<uchar>(Y, X) != 255) RETURN;   /* This pixel has already been labelled(0) (라벨링 안된 초기값 : 255  */
	output.at<uchar>(Y, X) = labelNo;

	if (X > 0) CALL_LabelComponent(X - 1, Y, 1);   /* left  pixel */
RETURN1:

	if (X < width - 1) CALL_LabelComponent(X + 1, Y, 2);   /* right pixel */
RETURN2:

	if (Y > 0) CALL_LabelComponent(X, Y - 1, 3);   /* upper pixel */
RETURN3:

	if (Y < height - 1) CALL_LabelComponent(X, Y + 1, 4);   /* lower pixel */
RETURN4:

	RETURN;
}

void LabelImage(unsigned short width, unsigned short height, Mat &input, Mat &output)
{
	printf("LabelImage : %d %d\n", width, height);
	unsigned short* STACK = (unsigned short*)malloc(3 * sizeof(unsigned short) * (width * height + 1));

	int labelNo = 0;
	//int index = -1;
	for (unsigned short y = 0; y < height; y++)
	{
		for (unsigned short x = 0; x < width; x++)
		{
			//index++;
			if (input.at<uchar>(y, x) == 255) continue;   /* This pixel is not part of a component */
			if (output.at<uchar>(y, x) != 255) continue;   /* This pixel has already been labelled  */
			/* New component found */
			labelNo++;
			LabelComponent(STACK, width, height, input, output, labelNo, x, y);
		}
	}
	
	free(STACK);
}


