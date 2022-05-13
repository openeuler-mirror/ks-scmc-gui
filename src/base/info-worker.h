#ifndef INFOWorker_H
#define INFOWorker_H

#include <QFuture>
#include <QFutureWatcher>
#include <QMutex>
#include <QObject>
#include <QPair>
#include "def.h"

#include <grpcpp/grpcpp.h>

#include "rpc.h"
#include "user-configuration.h"

//extern std::string g_server_addr;

enum ErrCode
{
    ErrOK = 0,
    ErrCanceled = 1,            // "请求取消"
    ErrUnknown = 2,             // "未知错误"
    ErrInvalidArgument = 3,     // "参数错误"
    ErrDeadlineExceeded = 4,    // "请求超时"
    ErrNotFound = 5,            // "资源不存在"
    ErrAlreadyExists = 6,       // "资源冲突"
    ErrPermissionDenied = 7,    // "拒绝请求"
    ErrResourceExhausted = 8,   // ResourceExhausted framework error
    ErrFailedPrecondition = 9,  // FailedPrecondition
    ErrAborted = 10,            // Aborted
    ErrOutOfRange = 11,         // OutOfRange
    ErrUnimplemented = 12,      // Unimplemented framework error
    ErrInternal = 13,           // Internal, "内部错误" framework error | rpc failure
    ErrUnavailable = 14,        // Unavailable  // framework error
    ErrDataLoss = 15,           // DataLoss
    ErrUnauthenticated = 16,    // Unauthenticated, "用户请求未认证"
    ErrWrongPassword = 17,      // Errno_WrongPassword, "密码错误"

    ErrUserNotExist = -1,
    ErrStatus = -2,
};

struct downloadImageInfo
{
    std::string name;
    std::string version;
    std::string type;
    std::string checksum;
    std::string imageFile;
    int64_t filesize;
};

class InfoWorker : public QObject
{
    Q_OBJECT
public:
    static InfoWorker &getInstance()
    {
        static InfoWorker s_InfoWorker;
        return s_InfoWorker;
    }

    // node management
    void listNode();
    void createNode(const node::CreateRequest &);
    void removeNode(const std::vector<int64_t> &node_ids);
    void nodeStatus(const std::vector<int64_t> &node_ids);

    // container management
    void listContainer(const std::vector<int64_t> &node_ids, const bool all);
    void createContainer(const container::CreateRequest &);
    void containerStatus(const int64_t node_id);
    void containerInspect(const int64_t node_id, const std::string &container_id);
    void startContainer(const std::map<int64_t, std::vector<std::string>> &ids);
    void stopContainer(const std::map<int64_t, std::vector<std::string>> &ids);
    void killContainer(const std::map<int64_t, std::vector<std::string>> &ids);
    void restartContainer(const std::map<int64_t, std::vector<std::string>> &ids);
    void removeContainer(const std::map<int64_t, std::vector<std::string>> &ids);
    void updateContainer(const container::UpdateRequest &);
    void monitorHistory(int node_id, int start_time, int end_time, uint32_t interval, std::string container_id = "");

    // container template
    void listTemplate(const int perPage, const int nextPage, const std::string sort, const std::string likeSearch);
    void listTemplate();
    void inspectTemplate(int64_t id);
    void createTemplate(const container::CreateTemplateRequest &);
    void updateTemplate(const container::UpdateTemplateRequest &);
    void removeTemplate(QList<int64_t> ids);

    //container backup
    void listBackup(int nodeId, std::string containerId);
    void updateBackup(int id, std::string backupDesc);
    void createBackup(int nodeId, std::string containerId, std::string backupDesc);
    void resumeBackup(int nodeId, std::string containerId, int backupId);
    void removeBackup(int id);

    // network management
    void listNetwork(const int64_t);
    void connectNetwork(const network::ConnectRequest &);
    void disconnectNetwork(const int64_t node_id, std::string interface, std::string container_id);
    void listIPtables(const int64_t node_id, std::string container_id = "");
    void enableIPtables(const int64_t node_id, bool enable, std::string container_id = "");
    void createIPtables(const network::CreateIPtablesRequest &);
    void modifyIPtables(const network::ModifyIPtablesRequest &);
    void removeIPtables(const network::RemoveIPtablesRequest &);

    // image management
    void listImage(const int64_t);
    void listDBImage();
    void uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile);
    void updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile);
    void downloadImage(const int64_t &image_id, const QString &name, const QString &version, const QString &savePath);
    void checkImage(const int64_t image_id, const bool approve, const std::string reject_reason);
    void removeImage(const std::vector<int64_t> &image_ids);

    // user management
    void login(const std::string &username, const std::string &password);
    void logout();
    void updatePassword(const std::string oldPassword, const std::string newPassword);

    // logging management
    void listRuntimeLogging(const logging::ListRuntimeRequest &);

    void stopTransfer(QString name, QString version, bool isStop);
    bool isTransferStoped(QString name, QString version);

private:
    InfoWorker(QObject *parent = nullptr);
    ~InfoWorker();

    // node management
    static QPair<grpc::Status, node::ListReply> _listNode(const node::ListRequest &);
    static QPair<grpc::Status, node::CreateReply> _createNode(const node::CreateRequest &);
    static QPair<grpc::Status, node::RemoveReply> _removeNode(const node::RemoveRequest &);
    static QPair<grpc::Status, node::StatusReply> _nodeStatus(const node::StatusRequest &);

    // container management
    static QPair<grpc::Status, container::ListReply> _listContainer(const container::ListRequest &);
    static QPair<grpc::Status, container::CreateReply> _createContainer(const container::CreateRequest &);
    static QPair<grpc::Status, container::StatusReply> _containerStatus(const container::StatusRequest &);
    static QPair<grpc::Status, container::InspectReply> _containerInspect(const container::InspectRequest &);
    static QPair<grpc::Status, container::StartReply> _startContainer(const container::StartRequest &);
    static QPair<grpc::Status, container::StopReply> _stopContainer(const container::StopRequest &);
    static QPair<grpc::Status, container::KillReply> _killContainer(const container::KillRequest &);
    static QPair<grpc::Status, container::RestartReply> _restartContainer(const container::RestartRequest &);
    static QPair<grpc::Status, container::UpdateReply> _updateContainer(const container::UpdateRequest &);
    static QPair<grpc::Status, container::RemoveReply> _removeContainer(const container::RemoveRequest &);
    static QPair<grpc::Status, container::MonitorHistoryReply> _monitorHistory(const container::MonitorHistoryRequest &);

    //container template
    static QPair<grpc::Status, container::ListTemplateReply> _listTemplate(const container::ListTemplateRequest &);
    static QPair<grpc::Status, container::InspectTemplateReply> _inspectTemplate(const container::InspectTemplateRequest &);
    static QPair<grpc::Status, container::CreateTemplateReply> _createTemplate(const container::CreateTemplateRequest &);
    static QPair<grpc::Status, container::UpdateTemplateReply> _updateTemplate(const container::UpdateTemplateRequest &);
    static QPair<grpc::Status, container::RemoveTemplateReply> _removeTemplate(const container::RemoveTemplateRequest &);

    //container backup
    static QPair<grpc::Status, container::ListBackupReply> _listBackup(const container::ListBackupRequest &);
    static QPair<grpc::Status, container::UpdateBackupReply> _updateBackup(const container::UpdateBackupRequest &);
    static QPair<grpc::Status, container::CreateBackupReply> _createBackup(const container::CreateBackupRequest &);
    static QPair<grpc::Status, container::ResumeBackupReply> _resumeBackup(const container::ResumeBackupRequest &);
    static QPair<grpc::Status, container::RemoveBackupReply> _removeBackup(const container::RemoveBackupRequest &);

    // network management
    static QPair<grpc::Status, network::ListReply> _listNetwork(const network::ListRequest &);
    static QPair<grpc::Status, network::ConnectReply> _connectNetwork(const network::ConnectRequest &);
    static QPair<grpc::Status, network::DisconnectReply> _disconnectNetwork(const network::DisconnectRequest &);
    static QPair<grpc::Status, network::ListIPtablesReply> _listIPtables(const network::ListIPtablesRequest &);
    static QPair<grpc::Status, network::EnableIPtablesReply> _enableIPtables(const network::EnableIPtablesRequest &);
    static QPair<grpc::Status, network::CreateIPtablesReply> _createIPtables(const network::CreateIPtablesRequest &);
    static QPair<grpc::Status, network::ModifyIPtablesReply> _modifyIPtables(const network::ModifyIPtablesRequest &);
    static QPair<grpc::Status, network::RemoveIPtablesReply> _removeIPtables(const network::RemoveIPtablesRequest &);

    // image management
    static QPair<grpc::Status, image::ListReply> _listImage(const image::ListRequest &);
    static QPair<grpc::Status, image::ListDBReply> _listDBImage(const image::ListDBRequest &);
    static QPair<grpc::Status, image::UploadReply> _uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile);
    static QPair<grpc::Status, image::UpdateReply> _updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile);
    static QPair<grpc::Status, downloadImageInfo> _downloadImage(image::DownloadRequest &req, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath);
    static QPair<grpc::Status, image::ApproveReply> _checkImage(const image::ApproveRequest &);
    static QPair<grpc::Status, image::RemoveReply> _removeImage(const image::RemoveRequest &);

    // user management
    static QPair<grpc::Status, user::LoginReply> _login(const user::LoginRequest &);
    static QPair<grpc::Status, user::LogoutReply> _logout(const user::LogoutRequest &);
    static QPair<grpc::Status, user::UpdatePasswordReply> _updatePassword(const user::UpdatePasswordRequest &);

    // logging management
    static QPair<grpc::Status, logging::ListRuntimeReply> _listRuntimeLogging(const logging::ListRuntimeRequest &);

signals:
    // node management
    void listNodeFinished(const QPair<grpc::Status, node::ListReply> &);
    void createNodeFinished(const QPair<grpc::Status, node::CreateReply> &);
    void removeNodeFinished(const QPair<grpc::Status, node::RemoveReply> &);
    void nodeStatusFinished(const QPair<grpc::Status, node::StatusReply> &);

    // container management
    void listContainerFinished(const QPair<grpc::Status, container::ListReply> &);
    void createContainerFinished(const QPair<grpc::Status, container::CreateReply> &);
    void containerStatusFinished(const QPair<grpc::Status, container::StatusReply> &);
    void containerInspectFinished(const QPair<grpc::Status, container::InspectReply> &);
    void startContainerFinished(const QPair<grpc::Status, container::StartReply> &);
    void stopContainerFinished(const QPair<grpc::Status, container::StopReply> &);
    void killContainerFinished(const QPair<grpc::Status, container::KillReply> &);
    void restartContainerFinished(const QPair<grpc::Status, container::RestartReply> &);
    void updateContainerFinished(const QPair<grpc::Status, container::UpdateReply> &);
    void removeContainerFinished(const QPair<grpc::Status, container::RemoveReply> &);
    void monitorHistoryFinished(const QPair<grpc::Status, container::MonitorHistoryReply> &);

    // container template
    void listTemplateFinished(const QPair<grpc::Status, container::ListTemplateReply> &);
    void inspectTemplateFinished(const QPair<grpc::Status, container::InspectTemplateReply> &);
    void createTemplateFinished(const QPair<grpc::Status, container::CreateTemplateReply> &);
    void updateTemplateFinished(const QPair<grpc::Status, container::UpdateTemplateReply> &);
    void removeTemplateFinished(const QPair<grpc::Status, container::RemoveTemplateReply> &);

    //container backup
    void listBackupFinished(const QPair<grpc::Status, container::ListBackupReply> &);
    void updateBackupFinished(const QPair<grpc::Status, container::UpdateBackupReply> &);
    void createBackupFinished(const QPair<grpc::Status, container::CreateBackupReply> &);
    void resumeBackupFinished(const QPair<grpc::Status, container::ResumeBackupReply> &);
    void removeBackupFinished(const QPair<grpc::Status, container::RemoveBackupReply> &);

    // network management
    void listNetworkFinished(const QPair<grpc::Status, network::ListReply> &);
    void connectNetworkFinished(const QPair<grpc::Status, network::ConnectReply> &);
    void disconnectNetworkFinished(const QPair<grpc::Status, network::DisconnectReply> &);
    void listIPtablesFinished(const QPair<grpc::Status, network::ListIPtablesReply> &);
    void enableIPtablesFinished(const QPair<grpc::Status, network::EnableIPtablesReply> &);
    void createIPtablesFinished(const QPair<grpc::Status, network::CreateIPtablesReply> &);
    void modifyIPtablesFinished(const QPair<grpc::Status, network::ModifyIPtablesReply> &);
    void removeIPtablesFinished(const QPair<grpc::Status, network::RemoveIPtablesReply> &);

    // image management
    void listImageFinished(const QPair<grpc::Status, image::ListReply> &);
    void listDBImageFinished(const QPair<grpc::Status, image::ListDBReply> &);
    void uploadFinished(const QPair<grpc::Status, image::UploadReply> &);
    void updateFinished(const QPair<grpc::Status, image::UpdateReply> &);
    void downloadImageFinished(const QPair<grpc::Status, downloadImageInfo> &);
    void checkImageFinished(const QPair<grpc::Status, image::ApproveReply> &);
    void removeImageFinished(const QPair<grpc::Status, image::RemoveReply> &);
    void transferImageFinished(QString, QString);
    void transferImageStatus(ImageTransmissionStatus, QString, QString, int);

    // user management
    void loginFinished(const QPair<grpc::Status, user::LoginReply> &);
    void logoutFinished(const QPair<grpc::Status, user::LogoutReply> &);
    void updatePasswordFinished(const QPair<grpc::Status, user::UpdatePasswordReply> &);
    void sessinoExpire();

    // logging management
    void loggingRuntimeFinished(const QPair<grpc::Status, logging::ListRuntimeReply> &);

private:
    QMutex mutex;
    QMap<QString, bool> m_transferStatusMap;
};

#endif  // INFOWORKER_H
