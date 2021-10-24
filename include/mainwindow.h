#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QLabel>
#include <QNetworkAccessManager>

#include <QGraphicsScene>
#include <QAction>

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
  void LoginService();
  void DrawFaces(const QVector<FaceInfo>& faces);
  QVector<FaceInfo> ParseResponse(const QJsonArray& data);

  QPixmap current_image;
  QGraphicsScene* scene;
  Ui::MainWindow *ui;
  QVector<QPair<QPixmap, QVector<FaceInfo>>> data_images_faces;
  QVector<FaceInfo> face_info;
  size_t pos_picture;
  QString url;
  QString query_login;
  QString query_detect;
  QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
  QString token;
};

#endif // MAINWINDOW_H
