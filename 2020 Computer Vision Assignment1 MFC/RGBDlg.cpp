
// RGBDlg.cpp : ���� ����
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
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") //�ܼ�â ��¿� --> �����ϱ�

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


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CRGBDlg ��ȭ ����



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
	/* ���� ��ư �ڵ��
	ON_BN_CLICKED(IDC_Red_Btn, &CRGBDlg::OnBnClickedRedBtn)
	ON_BN_CLICKED(IDC_Green_Btn, &CRGBDlg::OnBnClickedGreenBtn)
	ON_BN_CLICKED(IDC_Blue_Btn, &CRGBDlg::OnBnClickedBlueBtn)
	ON_BN_CLICKED(IDC_Gray_Btn, &CRGBDlg::OnBnClickedGrayBtn)
	ON_BN_CLICKED(IDC_Btn_Otsu, &CRGBDlg::OnBnClickedBtnOtsu)
	*/
END_MESSAGE_MAP()


// CRGBDlg �޽��� ó����

BOOL CRGBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CRGBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CRGBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// Load ��ư Ŭ�� ��
void CRGBDlg::OnBnClickedImgSearch()
{
	static TCHAR BASED_CODE szFilter[] = _T("�̹��� ����(*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif |�������(*.*)|*.*||");
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

// save ��ư Ŭ�� ��, ����ȯ & �����ϴ� �κ� / // �� ���� => (X) Mat img_copy_r, img_copy_g, img_copy_b, img_copy_rg, img_copy_gg, img_copy_bg, img_copy_rgo, img_copy_ggo, img_copy_bgo = img.clone();
void CRGBDlg::OnBnClickedImgSave()
{
	
	//�߰� ����

	//��� ���� ����
	int nRows = img.rows;
	int nCols = img.cols;
	Mat img_copy(nRows, nCols, CV_8UC1); // Mat img(rows, cols, type)
	cvtToGray(img, img_copy, nRows, nCols); // cvtColor(img, img_copy, CV_BGR2GRAY);
	imwrite("img_gray.jpg", img_copy); 

	//����ȭ
	Otsu(img_copy, nRows, nCols);
	imwrite("img_Otsu.jpg", img_copy);

	//�������� ���� (morphologyEx)
	Mat kernel(5, 5, CV_8U, Scalar(1)); // Mat kernel = Mat::ones(5, 5, CV_8U);

	//Opening (��������) : erode(ħ��) -> dilate(��â)
	Mat img_tmp1 = img_copy.clone();
	Mat img_opening = img_copy.clone();
	Erode(img_copy, img_tmp1, kernel, nRows, nCols);
	Dilate(img_tmp1, img_opening, kernel, nRows, nCols);
	imwrite("img_opening5.jpg", img_opening);

	//Closing (��������) : dilate(��â) -> erode(ħ��)
	Mat img_tmp2 = img_copy.clone();
	Mat img_closing = img_copy.clone();
	Dilate(img_copy, img_tmp2, kernel, nRows, nCols);
	Erode(img_tmp2, img_closing, kernel, nRows, nCols);
	imwrite("img_closing5.jpg", img_closing);
	
	//Contour Tracing (���̺�)
	vector<vector<Point>> contours1;
	vector<Vec4i> hierarchy1;
	findContours(img_opening, contours1, hierarchy1, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat img_opening_ct(nRows, nCols, CV_8U, Scalar(255));
	img_opening_ct.setTo(cv::Scalar(255));
	drawContours(img_opening_ct, contours1,
		-1,    // ��� �ܰ��� �׸���
		Scalar(0), // �˰�
		2);    // �β��� 2��

	imwrite("img_opening_ct.jpg", img_opening_ct);

	vector<vector<Point>> contours2;
	vector<Vec4i> hierarchy2;
	findContours(img_closing, contours2, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat img_closing_ct(nRows, nCols, CV_8U, Scalar(255));
	img_closing_ct.setTo(cv::Scalar(255));
	drawContours(img_closing_ct, contours2,
		-1,    // ��� �ܰ��� �׸���
		Scalar(0), // �˰�
		2);    // �β��� 2��

	imwrite("img_closing_ct.jpg", img_closing_ct);

	
	
	/*
	// ���� ContourTracing �Լ�
	vector<Point> cp;
	ContourTracing(img_opening, 0, 0, cp, nCols, nRows);
	imwrite("img_opening_ct.jpg", img_opening);
	ContourTracing(img_closing, 0, 0, cp, nCols, nRows);
	imwrite("img_closing_ct.jpg", img_closing);
	*/
	

	//// ���ο� contour tracing �Լ�
	//LabelingwithBT(img_opening);
	//imwrite("img_opening_ct.jpg", img_opening);
	//LabelingwithBT(img_closing);
	//imwrite("img_closing_ct.jpg", img_closing);
	

	/* ���ο� ct �Լ� 2
	Mat img_opening_ct(nRows, nCols, CV_8U, Scalar(255));
	LabelImage(nCols, nRows, img_opening, img_opening_ct);
	imwrite("img_opening_ct.jpg", img_opening_ct);
	*/


	/*
	//����2 (RGB & GRAY & Otsu)

	//1. changeColor & imwrite
	//red
	Mat img_copy_r = img.clone(); //Mat img_copy_r; �̷��� ���� �ϴ� �� �� �ȵ�.. cvtColor������ ���� �ϴ°� �����ѵ�.. &�ּ� ����..?
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
	Mat img_copy_rg; // ���� ���൵ ��
	cvtColor(img_copy_r, img_copy_rg, CV_BGR2GRAY); //cvtColor(���� �̹���(�Է�), ��ȯ�� �̹���(���), �÷� ��ȯ �ڵ�)
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
	


	//����3 (opening & closing ����)

	// ���� ȿ���� �� �ε巯���� 5x5 ���� ��Ҹ� ���
	Mat element5(5, 5, CV_8U, Scalar(1));
	
	//Opening (��������) : erode(ħ��) -> dilate(��â)
	Mat rgo_opening;    //���⼭ �� clone�� �����൵ �Ǵ�����...?
	morphologyEx(img_copy_rgo, rgo_opening, MORPH_OPEN, element5);
	//imwrite("rgo_opening.jpg", rgo_opening);

	Mat ggo_opening;
	morphologyEx(img_copy_ggo, ggo_opening, MORPH_OPEN, element5);
	//imwrite("ggo_opening.jpg", ggo_opening);

	Mat bgo_opening;
	morphologyEx(img_copy_bgo, bgo_opening, MORPH_OPEN, element5);
	//imwrite("bgo_opening.jpg", bgo_opening);


	//Closing (��������) : dilate(��â) -> erode(ħ��)
	Mat rgo_closing;    //���⼭ �� clone�� �����൵ �Ǵ�����...?
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
	//����4 (contour tracing)
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


	/* ���� �ڵ�
	imwrite("copy.jpg", img);
	MessageBox(_T("�̹��� ���� �Ϸ�!"), _T(""));
	//imshow();
	*/
}

/* ���� �ִ� ��ư�� �ڵ�
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
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
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
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
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
				// �̷��� RGB���� �����Ͽ� �� ���� �ȿ� �ִ� Rgb �ȼ����� �ܻ��� �־���.
				//200 -> 160 -> 110
				//�׸���
				//������ �ʷ����� �ص� ����� �ٲ�Ƿ� �Ժη� �մ��� ����
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
			uchar b = img.at<Vec3b>(i, j)[0]; // ������ �ȼ��� (y, x)��ǥ���� ���� �̹����� �ȼ����� ����
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
	//printf("%d", T); // T:�̹������� �ٸ�
	threshold(img_copy, binary, T, 255, CV_THRESH_BINARY);
	//DisplayImage(binary, 3);
	img_copy = binary.clone();
}


// Erode : ���� ���� �� �ȼ��� �� �ּ� �ȼ� ��(0)�� ���� �ȼ� ���� ����, �ּҰ�(min)���� ����
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

// Dilation : ���� ���� �� �ȼ��� �� �ִ� �ȼ� ��(255)�� ���� �ȼ� ���� ����, �ִ밪(max)���� ����
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
	// �ܰ��� ��ǥ�� ������ ����ü �ʱ�ȭ
	cp.clear();

	/* ***���� ��ü : ���(255) ==> �� �̹����� ��ü(0)���� �α�! �����κ�! (��ü �ּ� �Ʒ� �ڵ����)
	// �ܰ��� ���� ���� �ȼ��� ��ü(255)�� �ƴϸ� ����
	if (imgSrc.at<uchar>(sy, sx) != 255)
		return;
	*/

	int x, y, nx, ny;
	int d, cnt;
	int  dir[8][2] = { // ���� ������ ��Ÿ���� �迭
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
			// ���� ���⿡ �ִ� �ȼ��� ��ü(0)�� �ƴ� "���"�� ���(255),
			// �ð� �������� ���� ������ �ٲٰ� �ٽ� �õ��Ѵ�.

			if (++d > 7) d = 0;
			cnt++;

			// 8���� ��� ���(255)�� ��� 
			if (cnt >= 8)
			{
				cp.push_back(Point(x, y));
				break;  // �ܰ��� ������ ����.
			}
		}
		else
		{
			// ���� ������ �ȼ��� "��ü"�� ���, ���� ���� �ܰ��� ������ ����
			cp.push_back(Point(x, y));

			// ���� �������� �̵�
			x = nx;
			y = ny;

			// ���� ���� �ʱ�ȭ
			cnt = 0;
			d = (d + 6) % 8;	// d = d - 2 �� ���� ����
		}

		// ���������� ���ƿ԰�, ���� ������ �ʱ�ȭ�� ���
		// �ܰ��� ������ ������.
		if (x == sx && y == sy && d == 0) {
			imgSrc = Scalar(255); //Mat ��ü(�̹���) 255(���)���� �ʱ�ȭ

			int len = cp.size(); //cp.size() : vector ����ü�� ����� �ܰ��� point�� ����(����ü ũ��) : ex.6050
			printf("%d\n", len);

			for (int i = 0; i < len; i++) {
				imgSrc.at<uchar>(cp[i].y, cp[i].x) = 0x000; //���������� �ܰ��� ĥ�ϱ� (point��)
			}

			/*
			for (int i = 100; i<200; i++) {
				printf("%d %d %d\n", cp[i].x, cp[i].y, cp[i]); // �ܰ��� ��ǥ�� ����غ���
			}
			*/
			break;
		}
			
	}
}


void read_neighbor8(int y, int x, int neighbor8[], Mat &bImage) {
	//uchar b = img.at<Vec3b>(y, x)[0]; ==> 3ä�� �̹����� �ȼ��� ��������
	//img.at<uchar>(y, x); ==> 1ä�� �̹����� �ȼ��� ��������
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


// LUT �׸�ó�� �ʱ�ȭ �̸� �صα�(������ case��)
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

	//int labImage[10000][10000] = { 0, }; // label ���� ++�ϸ� ����� �迭(3����..?), [HEIGHT][WIDTH] �̷��� ������ ũ�� ���� �Ұ�!
	
	//�޸� �����Ҵ�
	int** labImage = new int* [10000];
	for (int i = 0; i < 10000; i++) {
		labImage[i] = new int[10000];
		memset(labImage[i], 0, sizeof(int) * 10000);//0���� �ʱ�ȭ �ϴ� �κ�
	}

	/*
	//���� ��ü(?)�� �ʱ�ȭ (label�� �ʱ�ȭ) ==> 3���� ������ ���� start, propagation, hole ���� �ȼ� ������� ����ϴ°� �ƴ�..?
	for (int i = 0; i < MAX_SIZE; i++) { 
		num_region[i] = 0;
	}
	*/
	int labelnumber = 1;

	for (int i = 1; i < (HEIGHT - 1); i++) {
		for (int j = 1; j < (WIDTH - 1); j++) {
			int cur_p = bImage.at<uchar>(i, j); // ���� ��ġ �о����
			//printf("%d\n", cur_p); //==> ��� 255.. �� ���������� 0 (0 �ϳ��� �������� ����)
			if (cur_p == 0) {   // object
				int ref_p1 = labImage[i][j - 1]; // �� �տ���
				int ref_p2 = labImage[i - 1][j - 1]; // �� �밢�� ���� ��
				if (ref_p1 > 1) {   // propagation (�� �տ����� �ϴ� �̹� ���� �Ҵ� �Ǿ�����(�̹� ���̺� �Ҵ��), ���� ���� ���ǿ� ���� ����  
					//num_region[ref_p1]++;
					labImage[i][j] = ref_p1;
				}
				else  if ((ref_p1 == 0) && (ref_p2 >= 2)) {   // hole
					//num_region[ref_p2]++;
					labImage[i][j] = ref_p2;
					BTracing8(i, j, ref_p2, BACKWARD, bImage, labImage); // hole�̴ϱ� �ݴ������� tracing �ϵ���
				}
				else  if ((ref_p1 == 0) && (ref_p2 == 0)) { // region start (������)
					labelnumber++;
					//num_region[labelnumber]++;
					labImage[i][j] = labelnumber;
					BTracing8(i, j, labelnumber, FOREWARD, bImage, labImage); // �������̴ϱ� ���������� tracing
				}
			}
			//else labImage[i][j] = 0;   // background (���� 0���� �ʱ�ȭ �س��� ��)
		}
	}

	//�޸� ����
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

	// do~while��
	do {
		int neighbor8[8];
		read_neighbor8(y, x, neighbor8, bImage); //neighbbor8�� �迭�̸��̹Ƿ�, �迭�� ù �ּ� �Ѿ
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

	//index = X + width * Y; // 2���� �迭�� �ε����� 1����ó�� �÷��� ����� ��!
	if (input.at<uchar>(Y, X) == 255) RETURN;   /* This pixel is not part of a component */
	if (output.at<uchar>(Y, X) != 255) RETURN;   /* This pixel has already been labelled(0) (�󺧸� �ȵ� �ʱⰪ : 255  */
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


