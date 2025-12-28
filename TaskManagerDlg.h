#pragma once
#include "IMessageReceiver.h"

#define WM_LOG_UPDATE (WM_USER + 200)

class CTaskManagerDlg : public CDialogEx, public IMessageReceiver
{
public:
    CTaskManagerDlg(CWnd* pParent = nullptr);
    virtual void OnMessageArrived(const std::string& message) override;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TASKMANAGER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnLogUpdate(WPARAM wParam, LPARAM lParam);

    afx_msg void OnBnClickedBtnEngineStart();
    afx_msg void OnBnClickedBtnEngineStop();
    afx_msg void OnBnClickedBtnCalcMatrix();
    afx_msg void OnBnClickedBtnDoBackup();
    afx_msg void OnBnClickedBtnWebRequest();
    afx_msg void OnBnClickedBtnAlertSvc();
    afx_msg void OnBnClickedBtnAnalytics();
    afx_msg void OnBnClickedBtnDemoDeadlock();
    afx_msg void OnBnClickedBtnDemoSafe();

    DECLARE_MESSAGE_MAP()

private:
    HICON m_hIcon;
    CString m_txtMatrix;
    CString m_txtWeb;
    CString m_txtStats;
public:
    afx_msg void OnStnClickedTxtResultWeb();
};
