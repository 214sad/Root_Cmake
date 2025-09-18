// root_1.C - 优化后的ROOT GUI应用程序
#include <TGClient.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TRootEmbeddedCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TGFileDialog.h>
#include <TROOT.h>
#include <TInterpreter.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TList.h>
#include <TError.h>
#include <TClass.h>
#include <TCollection.h>

class MyMainFrame : public TGMainFrame {
private:
   TGTextEntry      *fFileEntry;    
   TGTextButton     *fPlotButton;   
   TGTextButton     *fBrowseButton;
   TGLabel          *fCurrentDir;   
   TRootEmbeddedCanvas *fCanvas;    
   TH1              *fHist;         
   TGCheckButton    *fIsScript;     

public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();
   void DoPlot();                   
   void DoBrowse();                
   void CleanupPreviousHistograms();
   Bool_t ValidateFilePath();       
   
   ClassDef(MyMainFrame, 0)
};

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) 
   : TGMainFrame(p, w, h), fHist(0)
{
   
   SetCleanup(kDeepCleanup);
   
   // 创建垂直框架用于放置控件
   TGVerticalFrame *vframe = new TGVerticalFrame(this, 500, 30);
   AddFrame(vframe, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5));
   
   // 显示当前工作目录
   TString currentDir = "current directory ";
   currentDir += gSystem->WorkingDirectory();
   fCurrentDir = new TGLabel(vframe, currentDir.Data());
   vframe->AddFrame(fCurrentDir, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
   
   // 创建水平框架用于放置文件路径控件
   TGHorizontalFrame *hframe = new TGHorizontalFrame(vframe, 500, 30);
   vframe->AddFrame(hframe, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5));
   
   // 创建文件路径标签
   TGLabel *label = new TGLabel(hframe, "file path");
   hframe->AddFrame(label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
   
   // 创建文件路径输入框
   fFileEntry = new TGTextEntry(hframe);
   fFileEntry->SetText("../example.C");  // 默认文件设置为.C文件
   fFileEntry->Resize(250, 22);
   hframe->AddFrame(fFileEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY, 5, 5, 5, 5));
   
   // 创建浏览按钮
   fBrowseButton = new TGTextButton(hframe, "Search");
   fBrowseButton->Connect("Clicked()", "MyMainFrame", this, "DoBrowse()");
   hframe->AddFrame(fBrowseButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 5, 5, 5, 5));
   
   // 创建绘制按钮
   fPlotButton = new TGTextButton(hframe, "Diaw Histograw!");
   fPlotButton->Connect("Clicked()", "MyMainFrame", this, "DoPlot()");
   hframe->AddFrame(fPlotButton, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 5, 5, 5, 5));
   
   // 创建复选框，指示是否为脚本
   fIsScript = new TGCheckButton(vframe, "This is C++ script!");
   fIsScript->SetState(kButtonDown); // 默认选中，因为您使用的是.C文件
   vframe->AddFrame(fIsScript, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
   
   // 创建嵌入式画布
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 600, 400);
   AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
   
   // 设置窗口标题和大小
   SetWindowName("ROOT直方图查看器");
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
}

MyMainFrame::~MyMainFrame()
{
   // 清理资源
   if (fHist) delete fHist;
   Cleanup();
}

Bool_t MyMainFrame::ValidateFilePath()
{
   // 获取文件路径并验证
   TString filename = fFileEntry->GetText();
   
   // 检查路径是否为空
   if (filename.Length() == 0) {
      new TGMsgBox(gClient->GetRoot(), this, "ERROR", "path can not empty", kMBIconExclamation, kMBOk, 0);
      return kFALSE;
   }
   
   // 检查路径是否包含非法字符
   if (filename.Contains("\\") || filename.Contains("\"") || filename.Contains("'")) {
      new TGMsgBox(gClient->GetRoot(), this, "ERROR", "file path have char", kMBIconExclamation, kMBOk, 0);
      return kFALSE;
   }
   
   return kTRUE;
}

void MyMainFrame::CleanupPreviousHistograms()
{
   // 清理之前创建的直方图对象
   TList* list = gROOT->GetList();
   TIter next(list);
   TObject* obj;
   
   // 创建一个要删除的对象列表
   TList objectsToDelete;
   
   while ((obj = next())) {
      if (obj->InheritsFrom("TH1") && obj != fHist) {
         // 不删除当前显示的直方图，只删除其他直方图
         objectsToDelete.Add(obj);
      }
   }
   
   // 删除找到的直方图对象
   TIter deleteIter(&objectsToDelete);
   while ((obj = deleteIter())) {
      list->Remove(obj);
      delete obj;
   }
}

void MyMainFrame::DoBrowse()
{
   // 创建文件对话框
   TGFileInfo fileInfo;
   
   // 调整文件类型顺序，将.C文件放在前面
   const char *fileTypes[] = {
      "C++ files", "*.C;*.cpp;*.cxx",
      "ROOT files", "*.root",
      "All files", "*",
      0, 0
   };
   
   fileInfo.fFileTypes = fileTypes;
   fileInfo.fIniDir = StrDup(gSystem->WorkingDirectory());
   
   // 设置默认选择.C文件
   fileInfo.fFileTypeIdx = 0;
   
   new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fileInfo);
   
   // 如果用户选择了文件，更新文件路径输入框
   if (fileInfo.fFilename) {
      fFileEntry->SetText(fileInfo.fFilename);
      
      // 根据文件扩展名自动设置复选框状态
      TString filename = fileInfo.fFilename;
      if (filename.EndsWith(".C") || filename.EndsWith(".cpp") || filename.EndsWith(".cxx")) {
         fIsScript->SetState(kButtonDown);
      } else if (filename.EndsWith(".root")) {
         fIsScript->SetState(kButtonUp);
      }
   }
}

void MyMainFrame::DoPlot()
{
   // 验证文件路径
   if (!ValidateFilePath()) {
      return;
   }
   
   // 获取文件路径
   TString filename = fFileEntry->GetText();
   
   // 检查是否为脚本
   if (fIsScript->GetState() == kButtonDown) {
      // 运行C++脚本
      printf("运行脚本: %s\n", filename.Data());
      
      // 检查脚本是否存在
      if (gSystem->AccessPathName(filename)) {
         printf("脚本 %s 不存在!\n", filename.Data());
         
         // 显示错误信息
         TString msg = "scrpt ";
         msg += filename;
         msg += " not exit";
         new TGMsgBox(gClient->GetRoot(), this, "ERROR", msg.Data(), kMBIconExclamation, kMBOk, 0);
         
         return;
      }
      
      // 保存当前画布状态
      TVirtualPad *savedPad = gPad;
      
      // 设置当前画布为嵌入式画布
      fCanvas->GetCanvas()->cd();
      
      // 禁用自动创建新画布
      Bool_t oldBatch = gROOT->IsBatch();
      gROOT->SetBatch(kTRUE);
      
      // 禁用ROOT警告信息
      Int_t oldErrorLevel = gErrorIgnoreLevel;
      gErrorIgnoreLevel = kError;
      
      // 清理之前的直方图对象
      CleanupPreviousHistograms();
      
      // 删除当前显示的直方图
      if (fHist) {
         delete fHist;
         fHist = 0;
      }
      
      // 运行脚本
      Int_t error = 0;
      gROOT->ProcessLine(Form(".x %s", filename.Data()), &error);
      
      // 恢复错误级别设置
      gErrorIgnoreLevel = oldErrorLevel;
      
      // 恢复批处理模式设置
      gROOT->SetBatch(oldBatch);
      
      // 检查脚本是否运行成功
      if (error != 0) {
         printf("脚本运行失败: %s\n", filename.Data());
         
         // 显示错误信息
         TString msg = "run sript failed: ";
         msg += filename;
         msg += "\n check script ";
         new TGMsgBox(gClient->GetRoot(), this, "ERROR", msg.Data(), kMBIconExclamation, kMBOk, 0);
         
         // 恢复之前的画布
         if (savedPad) savedPad->cd();
         return;
      }
      
      // 尝试从内存中获取最新创建的直方图
      TH1 *newHist = 0;
      TList* list = gROOT->GetList();
      TIter next(list);
      TObject *obj;
      
      // 逆序遍历以获取最新创建的直方图
      while ((obj = next())) {
         if (obj->InheritsFrom("TH1")) {
            newHist = (TH1*)obj;
            // 继续查找以获取最后一个（最新）直方图
         }
      }
      
      if (!newHist) {
         printf("脚本未创建任何直方图!\n");
         
         // 显示错误信息
         TString msg = "script not create any TH!\n";
         msg += "sure create object!";
         new TGMsgBox(gClient->GetRoot(), this, "ERROR", msg.Data(), kMBIconExclamation, kMBOk, 0);
         
         // 恢复之前的画布
         if (savedPad) savedPad->cd();
         return;
      }
      
      // 更新当前直方图指针
      fHist = newHist;
      
      // 清理其他直方图（只保留当前显示的）
      CleanupPreviousHistograms();
      
      // 在嵌入式画布上绘制直方图
      fCanvas->GetCanvas()->cd();
      fCanvas->GetCanvas()->Clear();
      fHist->Draw();
      fCanvas->GetCanvas()->Update();
      
      // 恢复之前的画布
      if (savedPad) savedPad->cd();
      
      // 更新窗口标题以显示当前脚本
      TString title = "ROOT直方图查看器 - 脚本: ";
      title += filename;
      SetWindowName(title.Data());
      
      return;
   }
   
   // 以下是处理ROOT文件的代码
   
   // 检查文件是否存在
   if (gSystem->AccessPathName(filename)) {
      printf("文件 %s 不存在!\n", filename.Data());
      
      // 显示错误信息
      TString msg = "file ";
      msg += filename;
      msg += "not exit";
      new TGMsgBox(gClient->GetRoot(), this, "ERROR", msg.Data(), kMBIconExclamation, kMBOk, 0);
      
      return;
   }
   
   TFile *file = TFile::Open(filename);
   if (!file || file->IsZombie()) {
      printf("无法打开文件 %s!\n", filename.Data());
      
      // 显示错误信息
      TString msg = "can not file ";
      msg += filename;
      msg += "!";
      new TGMsgBox(gClient->GetRoot(), this, "ERROR", msg.Data(), kMBIconExclamation, kMBOk, 0);
      
      return;
   }
   
   // 获取第一个TH1对象
   TKey *key;
   TIter next(file->GetListOfKeys());
   TH1 *newHist = 0;
   while ((key = (TKey*)next())) {
      if (strcmp(key->GetClassName(), "TH1F") == 0 || 
          strcmp(key->GetClassName(), "TH1D") == 0 ||
          strcmp(key->GetClassName(), "TH1I") == 0 ||
          strcmp(key->GetClassName(), "TH2F") == 0 ||
          strcmp(key->GetClassName(), "TH2D") == 0 ||
          strcmp(key->GetClassName(), "TH2I") == 0 ||
          strcmp(key->GetClassName(), "TH3F") == 0 ||
          strcmp(key->GetClassName(), "TH3D") == 0 ||
          strcmp(key->GetClassName(), "TH3I") == 0) {
         newHist = (TH1*)key->ReadObj();
         break;
      }
   }
   
   // 检查是否找到直方图
   if (!newHist) {
      printf("in file %s can not th1\n", filename.Data());
      
      // 显示错误信息
      TString msg = "in file";
      msg += filename;
      msg += " cna not find TH1";
      new TGMsgBox(gClient->GetRoot(), this, "ERROR", msg.Data(), kMBIconExclamation, kMBOk, 0);
      
      file->Close();
      delete file;
      return;
   }
   
   // 清理之前的直方图
   CleanupPreviousHistograms();
   
   // 更新当前直方图指针
   if (fHist) delete fHist;
   fHist = newHist;
   
   // 在嵌入式画布上绘制直方图
   fCanvas->GetCanvas()->cd();
   fCanvas->GetCanvas()->Clear();
   fHist->Draw();
   fCanvas->GetCanvas()->Update();
   
   // 更新窗口标题以显示当前文件
   TString title = "ROOT直方图查看器 - ";
   title += filename;
   SetWindowName(title.Data());
   
   // 关闭文件
   file->Close();
   delete file;
}

// 创建应用程序实例并运行
void root_4()
{
   // 创建主窗口
   MyMainFrame *mainFrame = new MyMainFrame(gClient->GetRoot(), 800, 600);
}
