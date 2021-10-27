#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "network_functions.h"

#include <QFileDialog>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QBuffer>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , url("https://backend.facecloud.tevian.ru/")
  , query_login("api/v1/login")
  , query_detect("api/v1/detect")
  , scene(new QGraphicsScene)
{
  ui->setupUi(this);
  ui->progressBar->setValue(0);
  ui->next_picture_button->setVisible(false);
  ui->prev_picture_button->setVisible(false);
  ui->analys_button->setDisabled(true);
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
  QString selected_file = QFileDialog::getOpenFileName(this, 0,"", tr("Images (*.png *.jpg *.bmp)"));
  QPixmap image(selected_file);
  if (data_images_faces.empty()){
    data_images_faces.push_back({image, {}});
    pos_picture = 0;
    scene->addPixmap(data_images_faces[pos_picture].first);
    ui->graphicsView->setScene(scene);
    ui->analys_button->setEnabled(true);
  } else {
    data_images_faces.push_back({image, {}});
    ui->label_image_idx->setText("Изображение " + QString::number(pos_picture + 1) + '/' + QString::number(data_images_faces.size()));
  }
  ui->statusbar->showMessage("Добавлено 1 изображение");
}

void MainWindow::on_open_directory_action_triggered()
{
  QString selected_directory = QFileDialog::getExistingDirectory(this, 0,"");
  QDir directory(selected_directory);
  auto file_list = directory.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
  if (file_list.empty() && data_images_faces.empty()){
    ui->statusbar->showMessage("Не добавлено ни одного изображения.");
    ui->analys_button->setDisabled(true);
    ui->next_picture_button->setVisible(false);
    ui->prev_picture_button->setVisible(false);
    return;
  } else if (file_list.empty() && !data_images_faces.empty()){
    ui->statusbar->showMessage("Не добавлено ни одного изображения.");
    return;
  } else if (!file_list.empty() && !data_images_faces.empty()){
    for (const auto& item : file_list){
      QString image_path = selected_directory + '/' + item;
      data_images_faces.push_back({QPixmap(image_path), {}});
    }
    ui->statusbar->showMessage("Добавлено " + QString::number(file_list.size()) + " изображения(-ий)");
    ui->label_image_idx->setText("Изображение " + QString::number(pos_picture + 1) + '/' + QString::number(data_images_faces.size()));
  } else {
    for (const auto& item : file_list){
      QString image_path = selected_directory + '/' + item;
      data_images_faces.push_back({QPixmap(image_path), {}});
    }
    pos_picture = 0;
    scene->addPixmap(data_images_faces[pos_picture].first);
    ui->graphicsView->setScene(scene);
    ui->analys_button->setEnabled(true);
    ui->next_picture_button->setVisible(true);
    ui->prev_picture_button->setVisible(true);
    ui->statusbar->showMessage("Добавлено " + QString::number(file_list.size()) + " изображения(-ий)");
    ui->label_image_idx->setText("Изображение " + QString::number(pos_picture + 1) + '/' + QString::number(data_images_faces.size()));
  }
}

void MainWindow::on_next_picture_button_clicked()
{
  if (pos_picture == data_images_faces.size() - 1){
    pos_picture = 0;
  } else {
    pos_picture++;
  }
  scene->clear();
  scene->addPixmap(data_images_faces[pos_picture].first);
  ui->graphicsView->setScene(scene);
  DrawFaces(data_images_faces[pos_picture].second);
  ui->label_image_idx->setText("Изображение " + QString::number(pos_picture + 1) + '/' + QString::number(data_images_faces.size()));
}


void MainWindow::on_prev_picture_button_clicked()
{
  if (pos_picture == 0){
    pos_picture = data_images_faces.size() - 1;
  } else {
    pos_picture--;
  }
  scene->clear();
  scene->addPixmap(data_images_faces[pos_picture].first);
  ui->graphicsView->setScene(scene);
  DrawFaces(data_images_faces[pos_picture].second);
  ui->label_image_idx->setText("Изображение " + QString::number(pos_picture + 1) + '/' + QString::number(data_images_faces.size()));
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
  auto response = POST(mgr, request, std::move(data));
  QJsonDocument response_json = QJsonDocument::fromJson(response.toUtf8());
  if (response_json["status_code"].toInt() == 200){
      token = response_json["data"]["access_token"].toString();
  }
}

void MainWindow::DrawFaces(const QVector<FaceInfo>& faces){
  for (const auto& item : faces){
    QGraphicsRectItem* rect = new QGraphicsRectItem(item.rect.x, item.rect.y, item.rect.width, item.rect.height);
    rect->setPen(QPen(Qt::green));

    QGraphicsRectItem* back_rect = new QGraphicsRectItem(item.rect.x, item.rect.y + item.rect.height + 5, 70, 30);
    back_rect->setBrush(Qt::green);

    QGraphicsSimpleTextItem* demo_info = new QGraphicsSimpleTextItem();
    demo_info->setText("Возраст:" + QString::number(item.demo_info.age) +
                       "\nПол:" + item.demo_info.gender);
    demo_info->setPos(back_rect->rect().x()+2, back_rect->rect().y());

    scene->addItem(rect);
    scene->addItem(back_rect);
    scene->addItem(demo_info);
  }
}

Faces MainWindow::ParseResponse(const QJsonArray& data){
  Faces result;
  for (size_t i = 0; i < data.size() ;i++) {
    auto item = data.at(i).toObject();
    auto bbox = item["bbox"].toObject();
    FaceInfo info;
    info.rect.height = bbox["height"].toInt();
    info.rect.width = bbox["width"].toInt();
    info.rect.x = bbox["x"].toInt();
    info.rect.y = bbox["y"].toInt();
    auto demo = item["demographics"].toObject();
    auto demo_age = demo["age"].toObject();
    info.demo_info.age = demo_age["mean"].toInt();
    info.demo_info.gender = demo["gender"].toString();
    result.push_back(std::move(info));
  }
  return result;
}

void MainWindow::on_analys_button_clicked()
{
  ui->analys_button->setDisabled(true);
  if (token.isEmpty()){
    LoginService();
  }
  pos_picture = 0;
  ui->progressBar->setValue(0);

  size_t pos = 0;
  while(true){
    if (pos != data_images_faces.size()){
      QByteArray bArray;
      QBuffer buffer(&bArray);
      buffer.open(QIODevice::WriteOnly);
      data_images_faces[pos].first.save(&buffer, "JPG");
      auto data = SendImage(mgr, url + query_detect, token, std::move(bArray));
      auto v_info = ParseResponse(data);
      data_images_faces[pos].second = std::move(v_info);
      ui->progressBar->setValue(ui->progressBar->value() + 1);
      ui->progressBar->setMaximum(data_images_faces.size());
    } else {
      break;
    }
    pos++;
  }
  DrawFaces(data_images_faces[pos_picture].second);
  ui->analys_button->setEnabled(true);
  ui->statusbar->showMessage("Все изображения обработаны.");
  ui->graphicsView->update();
  ui->menubar->setEnabled(true);
}


