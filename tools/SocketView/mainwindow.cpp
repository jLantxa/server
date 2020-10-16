#include <numeric>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>

using server::comm::Message;
using server::comm::ServerMsgTypes::LOGIN;
using server::comm::ServerMsgTypes::LOGOUT;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(WINDOW_TITLE);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connect() {
    const std::string host = ui->teAddress->text().toStdString();
    const std::string address = ui->teAddress->text().toStdString();
    const uint16_t port = ui->tePort->text().toUInt();

    try {
        m_socket = new server::net::ClientSocket(server::net::Socket::Domain::IPv4,
                                             server::net::Socket::Type::STREAM,
                                             address, port);


        m_socket->Connect();

        m_isConnected = true;
        ui->wConnection->setEnabled(false);
        ui->wMessageView->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionClose->setEnabled(true);
        ui->bLogin->setEnabled(true);
        ui->bLogout->setEnabled(true);
    }
    catch(server::net::SocketException& socketException) {
        qDebug() << socketException.what();
    }
}

void MainWindow::disconnect() {
    sendLogout();

    if (m_socket != nullptr) {
        m_socket->Close();
        delete m_socket;
        m_socket = nullptr;
    }

    m_isConnected = false;
    ui->wConnection->setEnabled(true);
    ui->wMessageView->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionClose->setEnabled(false);
    ui->bLogin->setEnabled(false);
    ui->bLogout->setEnabled(false);
}

void MainWindow::calculate() {
    const std::string text = ui->tePayload->toPlainText().toStdString();
    const uint16_t size = text.size() + 1;
    const server::comm::MessageType type = ui->teMessageType->text().toUInt();

    const uint8_t sum = std::accumulate((uint8_t*) text.c_str(),
                                        (uint8_t*) text.c_str() + size - 1,
                                        size + type);
    const uint8_t checksum = 0xFF ^ sum;

    ui->teSize->setText(QString::number(size));
    ui->teChecksum->setText(QString::number(checksum));
}

void MainWindow::on_actionLOGIN_triggered() {
    if (m_isConnected == false) {
        return;
    } else {
        ui->teMessageType->setText(QString::number(server::comm::ServerMsgTypes::LOGIN));
    }
}

void MainWindow::on_bConnect_clicked() {
    connect();
}

void MainWindow::on_actionConnect_triggered() {
    connect();
}

void MainWindow::on_actionClose_triggered()
{
    disconnect();
}

void MainWindow::on_bSend_clicked() {
    if (m_socket == nullptr) {
        return;
    }

    const server::comm::MessageType type = ui->teMessageType->text().toUInt();
    const uint16_t size = ui->teSize->text().toUInt();
    const uint8_t checksum = ui->teChecksum->text().toUInt();
    const std::string text = ui->tePayload->toPlainText().toStdString();

    constexpr uint8_t typeSize = sizeof(server::comm::Message::Header::type);
    constexpr uint8_t sizeSize = sizeof(server::comm::Message::Header::size);
    constexpr uint8_t checksumSize = sizeof(server::comm::Message::Header::checksum);
    constexpr uint8_t headerSize = sizeof(server::comm::Message::Header);
    const uint16_t payloadSize = text.size() + 1;

    memcpy(m_buffer, &type, typeSize);
    memcpy(m_buffer + typeSize, &checksum, checksumSize);
    memcpy(m_buffer + typeSize + checksumSize, &size, sizeSize);
    memcpy(m_buffer + headerSize, text.c_str(), payloadSize);

    m_socket->Send(m_buffer, headerSize + payloadSize);
}

void MainWindow::on_bCalculate_clicked() {
    calculate();
}

void MainWindow::on_cbAutoCalc_stateChanged(int checked) {
    if (checked) {
        calculate();
    }
}

void MainWindow::on_tePayload_textChanged()
{
    if (ui->cbAutoCalc->checkState() == Qt::Checked) {
        calculate();
    }
}

void MainWindow::on_teMessageType_textChanged()
{
    if (ui->cbAutoCalc->checkState() == Qt::Checked) {
        calculate();
    }
}

void MainWindow::on_actionQuit_triggered() {
    disconnect();
    QApplication::quit();
}

void MainWindow::on_actionlocalhost_triggered() {
    static const char* LOCALHOST = "127.0.0.1";
    ui->teAddress->setText(LOCALHOST);
}

void MainWindow::on_bLogin_clicked() {
    sendLogin();
}

void MainWindow::on_bLogout_clicked() {
    sendLogout();
}

void MainWindow::sendLogin() {
    if (m_socket == nullptr) {
        return;
    }

    const std::string userToken = ui->tePayload->toPlainText().toStdString();
    Message logoutMsg(LOGIN, (uint8_t*) userToken.c_str(), userToken.size() + 1);
    logoutMsg.serialize(m_buffer, logoutMsg.getLength());
    m_socket->Send(m_buffer, logoutMsg.getLength());
}

void MainWindow::sendLogout() {
    if (m_socket == nullptr) {
        return;
    }

    Message logoutMsg(LOGOUT, nullptr, 0);
    logoutMsg.serialize(m_buffer, logoutMsg.getLength());
    m_socket->Send(m_buffer, logoutMsg.getLength());
}
