/*
 * 	This file is part of Nanomite.
 *
 *    Nanomite is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Nanomite is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Nanomite.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef QTDLGTRACE_H
#define QTDLGTRACE_H

#include "ui_qtDLGTrace.h"

#include <Windows.h>
#include <string>

#include <QList>

struct TraceInfoRow
{
	DWORD64 dwOffset;
	DWORD	PID;
	DWORD	TID;
	QString debugSymbols;
	//QString cpuReg;
};

class qtDLGTrace : public QWidget, public Ui_qtDLGTraceClass
{
	Q_OBJECT

public:
	qtDLGTrace(QWidget *parent = 0, Qt::WFlags flags = 0);
	~qtDLGTrace();
	
	static void addTraceData(DWORD64 dwOffset,DWORD PID,DWORD TID);
	static void clearTraceData();

signals:
	void OnDisplayDisassembly(quint64 Offset);

private:
	static qtDLGTrace *pThis;
	QList<TraceInfoRow> m_traceData;
	int m_iSelectedRow;

private slots:
	void OnShow(int Offset);
	void OnCustomContextMenu(QPoint qPoint);
	void MenuCallback(QAction* pAction);

protected:
	void showEvent(QShowEvent * event);
	void wheelEvent(QWheelEvent * event);
	void resizeEvent(QResizeEvent *event);
};

#endif