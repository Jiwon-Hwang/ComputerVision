
// RGBDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "RGB.h"
#include "RGBDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void changeColor(Mat img, Mat &copy, int i);
void Otsu(Mat& img_copy);


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
	//1. changeColor & imwrite
	//red
	Mat img_copy_r = img.clone(); //Mat img_copy_r; �̷��� ���� �ϴ� �� �� �ȵ�.. cvtColor������ ���� �ϴ°� �����ѵ�.. &�ּ� ����..?
	changeColor(img, img_copy_r, 1);
	//imwrite("copy_r.jpg", img_copy_r);

	//green
	Mat img_copy_g = img.clone();
	changeColor(img, img_copy_g, 2);
	//imwrite("copy_g.jpg", img_copy_g);

	//blue
	Mat img_copy_b = img.clone();
	changeColor(img, img_copy_b, 3);
	//imwrite("copy_b.jpg", img_copy_b);

	//red->gray
	Mat img_copy_rg; // ���� ���൵ ��
	cvtColor(img_copy_r, img_copy_rg, CV_BGR2GRAY); //cvtColor(���� �̹���(�Է�), ��ȯ�� �̹���(���), �÷� ��ȯ �ڵ�)
	//imwrite("copy_rg.jpg", img_copy_rg);

	//green->gray
	Mat img_copy_gg;
	cvtColor(img_copy_g, img_copy_gg, CV_BGR2GRAY); 
	//imwrite("copy_gg.jpg", img_copy_gg);

	//blue->gray
	Mat img_copy_bg;
	cvtColor(img_copy_b, img_copy_bg, CV_BGR2GRAY);
	//imwrite("copy_bg.jpg", img_copy_bg);


	//����3 ���� (Otsu���� -> opening & closing ����)

	// ���� ȿ���� �� �ε巯���� 5x5 ���� ��Ҹ� ���
	Mat element5(5, 5, CV_8U, Scalar(1));  

	//red->gray->Otsu
	Mat img_copy_rgo = img_copy_rg.clone();
	Otsu(img_copy_rgo); //Otsu(&img)
	imwrite("copy_rgo.jpg", img_copy_rgo);

	Mat rgo_opening;
	morphologyEx(img_copy_rgo, rgo_opening, MORPH_OPEN, element5);
	imshow("test", rgo_opening);
	imwrite("test", rgo_opening);

	//green->gray->Otsu
	Mat img_copy_ggo = img_copy_gg.clone();
	Otsu(img_copy_ggo); 
	imwrite("copy_ggo.jpg", img_copy_ggo);
	
	//blue->gray->Otsu
	Mat img_copy_bgo = img_copy_bg.clone();
	Otsu(img_copy_bgo);
	imwrite("copy_bgo.jpg", img_copy_bgo);


	//2. imshow
	/*
	imshow("Red", img_copy_r);
	imshow("Green", img_copy_g);
	imshow("Blue", img_copy_b);
	imshow("Red_Gray", img_copy_rg);
	imshow("Green_Gray", img_copy_gg);
	imshow("Blue_Gray", img_copy_bg);
	*/
	imshow("Red_Gray_Otsu", img_copy_rgo);
	imshow("Green_Gray_Otsu", img_copy_ggo);
	imshow("Blue_Gray_Otsu", img_copy_bgo);
	waitKey(0);
	destroyAllWindows();
	


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

void Otsu(Mat &img_copy)
{
	int nRows = img_copy.rows;
	int nCols = img_copy.cols;
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
	threshold(img_copy, binary, T, 255, CV_THRESH_BINARY);
	//DisplayImage(binary, 3);
	img_copy = binary.clone();
}






