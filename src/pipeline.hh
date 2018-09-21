#pragma once

#include "lsp_diagnostic.h"
#include "method.h"
#include "query.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct CompletionManager;
struct GroupMatch;
struct VFS;
struct Project;
struct WorkingFiles;
struct lsBaseOutMessage;

class DiagnosticsPublisher {
  std::unique_ptr<GroupMatch> match_;
  int64_t nextPublish_ = 0;
  int frequencyMs_;

 public:
  void Init();
  void Publish(WorkingFiles* working_files,
               std::string path,
               std::vector<lsDiagnostic> diagnostics);
};

struct VFS {
  struct State {
    int64_t timestamp;
    int step;
    bool loaded;
  };
  std::unordered_map<std::string, State> state;
  std::mutex mutex;

  void Clear();
  bool Loaded(const std::string &path);
  bool Stamp(const std::string &path, int64_t ts, int step);
};

namespace ccls {
enum class IndexMode {
  NonInteractive,
  OnChange,
  Normal,
};

namespace pipeline {
extern int64_t loaded_ts, tick;
void Init();
void LaunchStdin();
void LaunchStdout();
void Indexer_Main(CompletionManager *completion, VFS *vfs, Project *project,
                  WorkingFiles *wfiles);
void MainLoop();

void Index(const std::string &path, const std::vector<const char *> &args,
           IndexMode mode, lsRequestId id = {});

std::optional<std::string> LoadIndexedContent(const std::string& path);
void WriteStdout(MethodType method, lsBaseOutMessage &response);
} // namespace pipeline
} // namespace ccls
