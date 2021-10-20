#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QLabel>
#include <QNetworkAccessManager>

#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Rectangle{
  int x;
  int y;
  int height;
  int width;
};

struct Demographics{
  int age;
  QString gender;
};

struct FaceInfo{
  Rectangle rect;
  Demographics demo_info;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);


  ~MainWindow();

private slots:
  void on_exit_action_triggered();

  void on_open_picture_action_triggered();

  void on_open_directory_action_triggered();

  void on_next_picture_button_clicked();

  void on_prev_picture_button_clicked();

  void on_analys_button_clicked();

private:
  QString GET(const QNetworkRequest& request);
  QString POST(const QNetworkRequest& request, QByteArray&& postData);

  QJsonArray SendImage(QByteArray&& image);
  void LoginService();


  QPixmap current_image;

  Ui::MainWindow *ui;
  QVector<QPixmap> data_images;
  QVector<FaceInfo> face_info;
  QVector<QPixmap>::iterator cur_picture;
  size_t pos_picture;
  QString url;
  QString query_login;
  QString query_detect;
  QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
  QString token;
};
#endif // MAINWINDOW_H
