// SocketServerDlg.h : header file
//

#pragma once
#include <afxwin.h>
#include <atlbase.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <list>
#pragma warning(pop)
#include "CritSection.h"
#include "SocketHandle.h"
#include "SocketServerImpl.h"
#if defined(SOCKHANDLE_USE_OVERLAPPED)
#include "AsyncSocketServerImpl.h"
#endif

typedef std::list<SOCKET> CSocketList;

// CSocketServerDlg dialog
class CSocketServerDlg : public CDialog,
                         public ISocketServerHandler
{
#if defined(SOCKHANDLE_USE_OVERLAPPED)
    typedef ASocketServerImpl<ISocketServerHandler> CSocketServer;
#else
    typedef SocketServerImpl<ISocketServerHandler> CSocketServer;
#endif

    friend CSocketServer;
// Construction
public:
    CSocketServerDlg(CWnd* pParent = NULL); // standard constructor
    virtual ~CSocketServerDlg();

// Dialog Data
    enum { IDD = IDD_SOCKETSERVER_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // ISocketServerHandler
    virtual void OnThreadBegin(CSocketHandle* pSH);
    virtual void OnThreadExit(CSocketHandle* pSH);
    virtual void OnAddConnection(CSocketHandle* pSH, SOCKET newSocket);
    virtual void OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr);
    virtual void OnConnectionFailure(CSocketHandle* pSH, SOCKET newSocket);
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

    CSocketServer m_SocketServer;

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
    afx_msg void OnBnClickedBtnStart();
    afx_msg void OnBnClickedBtnStop();
    afx_msg void OnBnClickedBtnSend();
};
