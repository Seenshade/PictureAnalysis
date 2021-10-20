#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QFileDialog"

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QEventLoop>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

#include <QBuffer>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , url("https://backend.facecloud.tevian.ru/")
  , query_login("api/v1/login")
  , query_detect("api/v1/detect")
  , picture_label(new QLabel)
{
  ui->setupUi(this);
  ui->next_picture_button->setVisible(false);
  ui->prev_picture_button->setVisible(false);

  picture_label->setBackgroundRole(QPalette::Base);
//  picture_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  picture_label->setScaledContents(true);

  ui->scrollArea->setBackgroundRole(QPalette::Dark);
  ui->scrollArea->setWidget(picture_label);
  ui->scrollArea->setVisible(false);
  ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_exit_action_triggered()
{
  close();
}


void MainWindow::on_open_picture_action_triggered()
{
  data_images.clear();
  QString selected_file = QFileDialog::getOpenFileName(this, 0,"", tr("Images (*.png *.jpg *.bmp)"));
  QPixmap image(selected_file);
//  image = image.scaled(picture_label->width(), picture_label->height(),Qt::IgnoreAspectRatio);
  data_images.push_back(image);

//  picture_label->resize(picture_label->pixmap().size());
  ui->scrollArea->setEnabled(true);
  ui->scrollArea->show();
    picture_label->setPixmap(*data_images.begin());
}


void MainWindow::on_open_directory_action_triggered()
{
  data_images.clear();
  QString selected_directory = QFileDialog::getExistingDirectory(this, 0,"");
  QDir directory(selected_directory);
  auto file_list = directory.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
  if (file_list.size() > 1){
      ui->next_picture_button->setVisible(true);
      ui->prev_picture_button->setVisible(true);
    }
  ui->statusbar->showMessage("Обнаружено " + QString::number(file_list.size())+ " изображений");
  for (const auto& item : file_list){
      QString image_path = selected_directory + '/' + item;
      data_images.push_back(QPixmap(image_path));
    }
  cur_picture = data_images.begin();
  pos_picture = 1;
  ui->PictureBox->setTitle("Изображение " + QString::number(pos_picture) + '/' + QString::number(data_images.size()));
  picture_label->setPixmap(*cur_picture);
}

void MainWindow::on_next_picture_button_clicked()
{
  if (pos_picture == data_images.size()){
      pos_picture = 1;
      cur_picture = data_images.begin();
    } else {
      cur_picture++ = std::next(cur_picture);
      pos_picture++;
    }
  picture_label->setPixmap(*cur_picture);
  ui->PictureBox->setTitle("Изображение " + QString::number(pos_picture) + '/' + QString::number(data_images.size()));
}


void MainWindow::on_prev_picture_button_clicked()
{
  if (pos_picture == 1){
      pos_picture = data_images.size();
      cur_picture = std::prev(data_images.end());
    } else {
      cur_picture = std::prev(cur_picture);
      pos_picture--;
    }
  picture_label->setPixmap(*cur_picture);
  ui->PictureBox->setTitle("Изображение " + QString::number(pos_picture) + '/' + QString::number(data_images.size()));
}

QString MainWindow::GET(const QNetworkRequest& request){
  QNetworkReply* reply = mgr->get(request);
  QEventLoop loop;
  connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  reply->deleteLater();
  QString response = QString::fromUtf8(reply->readAll());
  return response;
}

QString MainWindow::POST(const QNetworkRequest& request, QByteArray&& postData)
{
  QNetworkReply* reply = mgr->post(request, postData);
  QEventLoop loop;
  connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  QString response = QString::fromUtf8(reply->readAll());
  return response;
}

void MainWindow::LoginService(){
  QJsonObject obj{
    {"email", "seenshade.sb@gmail.com"},
    {"password", "test1234"}
  };
  QJsonDocument doc(std::move(obj));
  QString strJson(std::move(doc).toJson(QJsonDocument::Compact));
  auto data = std::move(strJson).toUtf8();

  QNetworkRequest request(url + query_login);
  request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
  auto response = POST(request, std::move(data));

  QJsonDocument response_json = QJsonDocument::fromJson(response.toUtf8());
  if (response_json["status_code"].toInt() == 200){
      token = response_json["data"]["access_token"].toString();
  }
}

QJsonArray MainWindow::SendImage(QByteArray&& image){
  QUrl url_query(url + query_detect);
  url_query.setQuery("demographics=true");
  QNetworkRequest request(url_query.toString());
  request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
  request.setHeader(QNetworkRequest::ContentTypeHeader, "image/jpeg");
  request.setRawHeader("accept", "application/json");
  auto response = POST(request, std::move(image));
  QJsonDocument response_json = QJsonDocument::fromJson(response.toUtf8());
  auto data = response_json["data"].toArray();
  return data;
}

void MainWindow::on_analys_button_clicked()
{
  LoginService();
  QByteArray bArray;
  QBuffer buffer(&bArray);
  buffer.open(QIODevice::WriteOnly);
  cur_picture->save(&buffer, "JPG");
  auto data = SendImage(std::move(bArray));
  for (size_t i = 0; i < data.size() ;i++) {
    auto item = data.at(i).toObject();
    auto bbox = item["bbox"].toObject();
    FaceInfo info;
    info.rect.height = bbox["height"].toInteger();
    info.rect.width = bbox["width"].toInteger();
    info.rect.x = bbox["x"].toInteger();
    info.rect.y = bbox["y"].toInteger();
    auto demo = item["demographics"].toObject();
    info.demo_info.age = demo["age"].toInt();
    info.demo_info.gender = demo["gender"].toString();
    face_info.emplace_back(info);
  }

}


