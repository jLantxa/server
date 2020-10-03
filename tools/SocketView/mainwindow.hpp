#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <queue>

#include "Communication.hpp"
#include "net/Socket.hpp"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionLOGIN_triggered();
    void on_bConnect_clicked();
    void on_actionConnect_triggered();
    void on_actionClose_triggered();
    void on_bSend_clicked();
    void on_bCalculate_clicked();
    void on_cbAutoCalc_stateChanged(int checked);
    void on_tePayload_textChanged();
    void on_teMessageType_textChanged();
    void on_actionQuit_triggered();
    void on_actionlocalhost_triggered();
    void on_bLogin_clicked();
    void on_bLogout_clicked();

private:
    Ui::MainWindow *ui;
    const char* WINDOW_TITLE = "SocketView";
    static const uint16_t BUFFER_SIZE = 1024;

    server::net::ClientSocket* m_socket;
    bool m_isConnected = false;
    std::queue<server::comm::Message> m_messageQueue;
    uint8_t m_buffer[BUFFER_SIZE];

    void connect();
    void disconnect();
    void calculate();

    void sendLogin();
    void sendLogout();
};
#endif // MAINWINDOW_HPP
