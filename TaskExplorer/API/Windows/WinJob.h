#pragma once
#include <qobject.h>
#include "../../Common/FlexError.h"
#include "../ProcessInfo.h"
#include "../AbstractInfo.h"

struct SJobStats
{
	SJobStats()
	{
		LastStatUpdate = 0;
	}

	bool UpdateStats()
	{
		quint64 curTick = GetCurTick();
		quint64 time_ms = curTick - LastStatUpdate;
		LastStatUpdate = curTick;

		Io.UpdateStats(time_ms);

		return true;
	}

	SDelta64	KernelDelta;
	SDelta64	UserDelta;

	SDelta32 	PageFaultsDelta;

	quint64		LastStatUpdate;

	SIOStatsEx	Io;
};

class CWinJob : public CAbstractInfo
{
	Q_OBJECT
public:
	CWinJob(QObject *parent = nullptr);
	virtual ~CWinJob();

	static CWinJob*	JobFromHandle(quint64 ProcessId, quint64 HandleId);

	virtual QString			GetJobName() const { QReadLocker Locker(&m_Mutex); return m_JobName; }

	virtual QMap<quint64, CProcessPtr>	GetProcesses() const { QReadLocker Locker(&m_Mutex); return m_Processes; }

	virtual quint32			GetActiveProcesses() const { QReadLocker Locker(&m_Mutex); return m_ActiveProcesses; }
	virtual quint32			GetTotalProcesses() const { QReadLocker Locker(&m_Mutex); return m_TotalProcesses; }
	virtual quint32			GetTerminatedProcesses() const { QReadLocker Locker(&m_Mutex); return m_TerminatedProcesses; }

	virtual quint32			GetPeakProcessMemoryUsed() const { QReadLocker Locker(&m_Mutex); return m_PeakProcessMemoryUsed; }
	virtual quint32			GetPeakJobMemoryUsed() const { QReadLocker Locker(&m_Mutex); return m_PeakJobMemoryUsed; }

	virtual SJobStats		GetStats() const { QReadLocker Locker(&m_Mutex);  return m_Stats; }

	virtual STATUS			Terminate();
	virtual STATUS			AddProcess(quint64 ProcessId);

	virtual void OpenPermissions();

	enum EQueryType
	{
		eProcess = 0,
		eHandle
	};

protected:
	friend class CWinProcess;
	friend class CJobView;

	bool InitStaticData(void* QueryHandle, EQueryType Type = eProcess);
	bool UpdateDynamicData();

	QString				m_JobName;

	quint32				m_ActiveProcesses;
	quint32				m_TotalProcesses;
	quint32				m_TerminatedProcesses;

	quint64				m_PeakProcessMemoryUsed;
	quint64				m_PeakJobMemoryUsed;

	SJobStats			m_Stats;

	QMap<quint64, CProcessPtr>	m_Processes;

private:
	struct SWinJob*		m;
};


typedef QSharedPointer<CWinJob> CWinJobPtr;
