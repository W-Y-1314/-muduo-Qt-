#ifndef CHATBUBBLE_H_
#define CHATBUBBLE_H_

#include <QWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QLabel>
#include <vector>

class QPaintEvent;
class QResizeEvent;

/*
* 核心：重写 paintEvent、resizeEvent事件
*
* 冒泡框：大小固定，故就需要计算字体大小
*	左右边：其实就是不同绘图位置
*/

// 目的：仿照微信那种对话框
class ChatBubble : public QLabel, public QListWidgetItem
{
	Q_OBJECT
public:
	enum BubbleType {
		BubbleLeft,    // 显示在左边
		BubbleRight    // 显示在右边
	};
	ChatBubble(QWidget* parent = nullptr);
	ChatBubble(const QPixmap& head_img, const QString& message, ChatBubble::BubbleType type = ChatBubble::BubbleType::BubbleLeft, QWidget* parent = nullptr);  // 带图片初始化

	// 设置消息
	void setMessage(const QPixmap& head_img, const QString& message, ChatBubble::BubbleType type = ChatBubble::BubbleLeft);

	// 初始化页面
	void initUi();

protected:
	// 从写绘图事件
	void paintEvent(QPaintEvent* ev)override;
	// 从写控件大小事件
	void resizeEvent(QResizeEvent* ev)override;

private:
	// 更新气泡图大小
	void updateBubbleSize();
	//文本输出实际矩形区域, 需要更具文本长度自适应
	void updateTextRect();
	// 计算发送消息，每一行的宽度
	std::vector<int> textWidthList() const;
	int lineNumber() const;   // 消息文本总行数
	int realLineNumber() const;  // 实际行数量，去除空行

private:
	// 消息框在左边还是在右边, 默认右边，自己发
	BubbleType m_type = BubbleRight;
	QRect m_profileRect;		// 头像矩形
	QRect m_bubbleRect;         // 气泡矩形

	// 绘制小三角形
	QList<QPointF> m_points;

	// 文本矩阵
	QRect m_textRect;
	int m_textMargin = 12;   // 文本距离

	// 气泡图矩阵中，头像偏移量
	int m_xOffset = 0;
	int m_yOffset = 0;

	QPixmap m_head_img;    // 头像照片
	QString m_message;     // 要发送的消息
};

#endif // !CHATBUBBLE_H_