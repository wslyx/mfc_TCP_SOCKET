#pragma once
#include <afxwin.h>
#include <atlbase.h>
#include "SocketHandle.h"
#include "SocketClientImpl.h"

// CSocketClientDlg dialog

class CSocketClientDlg : public CDialog,
                         public ISocketClientHandler
{
// Construction
    typedef SocketClientImpl<ISocketClientHandler> CSocketClient;
    friend CSocketClient;
public:
    CSocketClientDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CSocketClientDlg();

// Dialog Data
    enum { IDD = IDD_SOCKETCLIENT_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // SocketClient Interface handler
    virtual void OnThreadBegin(CSocketHandle* pSH);
    virtual void OnThreadExit(CSocketHandle* pSH);
    virtual void OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr);
    virtual void OnConnectionDropped(CSocketHandle* pSH);
    virtual void OnConnectionError(CSocketHandle* pSH, DWORD dwError);

// Implementation
protected:

    HICON   m_hIcon;
    CString m_strPort;
    int m_nMode;
    int m_nSockType;
    CEdit m_ctlMessage;
    CEdit m_ctlMsgList;

    CSocketClient m_SocketClient;

    void EnableControl(UINT nCtrlID, BOOL bEnable);
    void SyncControls();
    void GetAddress(const SockAddrIn& addrIn, CString& rString) const;
    void _cdecl AppendText(LPCTSTR lpszFormat, ...);
    bool GetDestination(SockAddrIn& addrIn) const;
    bool SetupMCAST();

    // Generated message map functions
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnModeSelected(UINT nId);
    afx_msg void OnBnClickedBtnStart();
    afx_msg void OnBnClickedBtnStop();
    afx_msg void OnBnClickedBtnSend();
};
