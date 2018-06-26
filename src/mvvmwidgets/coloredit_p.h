#ifndef COLOREDIT_P_H
#define COLOREDIT_P_H

#include <QtGui/QColor>
#include <QtGui/QRegularExpressionValidator>
#include <QtWidgets/QWidget>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QLineEdit>

#include "qtmvvmwidgets_global.h"

namespace QtMvvm {

class ColorButton : public QAbstractButton
{
	Q_OBJECT

	Q_PROPERTY(QColor color READ color WRITE setColor)

public:
	explicit ColorButton(QWidget *parent = nullptr);

	QColor color() const;

	QSize sizeHint() const override;

public Q_SLOTS:
	void setColor(QColor color);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	QColor _color;
};

class ColorEdit : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)
	Q_PROPERTY(bool alpha READ useAlpha WRITE setAlpha NOTIFY alphaChanged)

public:
	explicit ColorEdit(QWidget *parent = nullptr);

	QColor color() const;
	bool useAlpha() const;

public Q_SLOTS:
	void setColor(QColor color);
	void setAlpha(bool alpha);

Q_SIGNALS:
	void colorChanged(const QColor &color);
	void alphaChanged(bool alpha);

private Q_SLOTS:
	void showDialog();
	void updateColor(const QString &text);

private:
	QColor _color;
	bool _alpha = true; //in order to be set to false in the constructor

	ColorButton *_btn;
	QLineEdit *_edit;
	QRegularExpressionValidator *_validator;
	bool _skipEditUpdate = false;
};

}

#endif // COLOREDIT_P_H
