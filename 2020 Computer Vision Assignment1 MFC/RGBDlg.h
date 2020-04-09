#include <opencv2/opencv.hpp>
#include "afxwin.h"
using namespace cv;
// RGBDlg.h : 헤더 파일
//

#pragma once


// CRGBDlg 대화 상자
class CRGBDlg : public CDialogEx
{
// 생성입니다.
public:
	CRGBDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	Mat img;
	CString pathName;
	CRect rect;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RGB_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedImgSearch();
	CStatic m_pic;
	//이미지 load용
	void CRGBDlg::DisplayImage(Mat targetMat, int channel);
	afx_msg void OnBnClickedImgSave();
	afx_msg void OnBnClickedRedBtn();
	afx_msg void OnBnClickedGreenBtn();
	afx_msg void OnBnClickedBlueBtn();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedGrayBtn();
	afx_msg void OnBnClickedBtnOtsu();
};
