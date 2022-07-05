//
//  ViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa

class ViewController: NSViewController, NSComboBoxDataSource {

    @IBOutlet var filePathTextField: NSTextField!
    @IBOutlet var vorLabel: NSTextField!
    @IBOutlet var uwndLabel: NSTextField!
    @IBOutlet var vwndLabel: NSTextField!
    @IBOutlet var zLvComboBox: NSComboBox!
    @IBOutlet var isWrfoutIcon: NSImageView!
    @IBOutlet var selVarNameBtn: NSButton!
    @IBOutlet var startTrackingBtn: NSButton!
    @IBOutlet var showWebBtn: NSButton!
    
    
    
    @objc private dynamic var timeVarStr = "未指定"
    @objc private dynamic var latVarStr = "未指定"
    @objc private dynamic var lonVarStr = "未指定"
    @objc private dynamic var vorVarStr = "未指定" {
        didSet {
            vorLabel.textColor = vorVarStr.isEmpty ? .gray : .labelColor
        }
    }
    @objc private dynamic var uwndVarStr = "未指定" {
        didSet {
            uwndLabel.textColor = uwndVarStr.isEmpty ? .gray : .labelColor
        }
    }
    @objc private dynamic var vwndVarStr = "未指定" {
        didSet {
            vwndLabel.textColor = vwndVarStr.isEmpty ? .gray : .labelColor
        }
    }
    @objc private dynamic var zVarStr = "未指定"
    
    @objc private dynamic var isWrfoutFile = false
    private var zLvDimLen: Int = 0
    /// 给getZLvDimLenName方法使用
    private var theVarStr: String {
        vorVarStr.isEmpty ? uwndVarStr : vorVarStr
    }
    private var realTCs: [Typhoon] = []
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        isWrfoutIcon.imageScaling = .scaleAxesIndependently
        zLvComboBox.dataSource = self
        NotificationCenter.default.addObserver(self, selector: #selector(getVarNames(_:)), name: NSNotification.Name(rawValue: "AllVarNamesGet"), object: nil)
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    @IBAction func browseBtnClicked(_ sender: NSButton) {
        let filePath = showFileBrowser()
        zLvComboBox.isEnabled = false
        guard let filePath = filePath else { return }

        checkFileValid(filePath: filePath)
        
        checkIfIsWrfoutFile(ncFilePath: filePath)
    }
    
    @IBAction func varSelBtnClicked(_ sender: NSButton) {
        guard let varSelectVC = storyboard?.instantiateController(withIdentifier: "VarSelectVC") as? VarSelectViewController else { return }
//        varSelectVC?.view.window?.windowController?.showWindow(self)
        varSelectVC.ncFilePath = filePathTextField.stringValue
//        NSApp.runModal(for: varSelectWC.window!)
//        presentAsModalWindow(varSelectVC)
        presentAsSheet(varSelectVC)

    }
    
    @IBAction func startTrackBtnClicked(_ sender: NSButton) {
        if !checkZLvCombox() { return }
        
        realTCs = NCFileInfo_Wrapper(ncFilePath: filePathTextField.stringValue, isWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, uwndVarStr, vwndVarStr, !vorVarStr.isEmpty, (zLvDimLen == 0) ? -1 : zLvComboBox.intValue, "/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/out/").startTracking().compactMap { $0 as? Typhoon }
        
        
        
        
        
    }
    
    @IBAction func showWebBtnClicked(_ sender: NSButton) {
        guard let resultVC = storyboard?.instantiateController(withIdentifier: "ResultView") as? ResultViewController else { return }
        resultVC.tcsData = realTCs
        presentAsModalWindow(resultVC);
    }
    
    
    private func checkZLvCombox() -> Bool {
        if zLvDimLen == 0 { return true }
        let selectedIndex = zLvComboBox.indexOfSelectedItem
        if selectedIndex == -1 {
            let comboBoxValue = zLvComboBox.stringValue
            let alert = NSAlert()
            alert.messageText = comboBoxValue.isEmpty ? "层数不能为空！" : "\(comboBoxValue): 输入的层数不合法。"
            alert.runModal()
            return false
        }
        return true
    }
    
    
    @objc private func getVarNames(_ notification: NSNotification) {
        guard let userInfo = notification.userInfo, let varNames = userInfo["varNames"] as? [String] else { return }
        setVarName(time: varNames[0], lat: varNames[1], lon: varNames[2], vor: varNames[3], u: varNames[4], v: varNames[5])
//        print("varNames:", notification.userInfo!["varNames"])
        
        handleZLevelDim()
        
        startTrackingBtn.isEnabled = true
    }
    
    private func checkIfIsWrfoutFile(ncFilePath: String) {
        var eInfo: NSString?;
        let isWrfoutFile = NCFileInfo_Wrapper(ncFilePath: ncFilePath).checkIfIsWrfoutFile(&eInfo)
        self.isWrfoutFile = isWrfoutFile
        if isWrfoutFile {
            setVarName(time: "XTIME", lat: "XLAT", lon: "XLONG", vor: "", u: "U", v: "V")
            isWrfoutIcon.image = NSImage(systemSymbolName: "checkmark.square", accessibilityDescription: nil)
            handleZLevelDim()
            selVarNameBtn.isEnabled = false
            startTrackingBtn.isEnabled = true
        } else {
            isWrfoutIcon.image = NSImage(systemSymbolName: "multiply.square", accessibilityDescription: nil)
            setVarName(allStr: "未指定")
            selVarNameBtn.isEnabled = true
        }
    }
    
    private func setVarName(time: String, lat: String, lon: String, vor: String, u: String, v: String) {
        timeVarStr = time
        latVarStr = lat
        lonVarStr = lon
        vorVarStr = vor
        uwndVarStr = u
        vwndVarStr = v
    }
    private func setVarName(allStr: String) {
        timeVarStr = allStr
        latVarStr = allStr
        lonVarStr = allStr
        vorVarStr = allStr
        uwndVarStr = allStr
        vwndVarStr = allStr
    }
    
    private func checkFileValid(filePath: String) {
        var fileValidInfo: NSString?
        let fileValid = NCFileInfo_Wrapper(ncFilePath: filePath).checkFileValid(&fileValidInfo)
        
        if !fileValid {
            let alert = NSAlert()
            alert.messageText = "The selected file is not vaild!"
            if let fileValidInfo = fileValidInfo {
                alert.informativeText = fileValidInfo as String
            }
            alert.runModal()
            filePathTextField.stringValue = ""
        } else {
            filePathTextField.stringValue = filePath
        }
    }
    
    private func handleZLevelDim() {
        var zLvDimName: NSString?
        zLvDimLen = Int(NCFileInfo_Wrapper(ncFilePath: filePathTextField.stringValue, timeVarStr, latVarStr, lonVarStr, vorVarStr, uwndVarStr, vwndVarStr, !vorVarStr.isEmpty).getZLvDimLenName(&zLvDimName))
        
        if zLvDimLen == 0 {
            zVarStr = "(无)"
            zLvComboBox.isEnabled = false
            return
        }
        zVarStr = zLvDimName as? String ?? ""
        
    }
    
    private func showFileBrowser() -> String? {
        let dialog = NSOpenPanel();

        dialog.title = "Choose a file";
        dialog.showsResizeIndicator = true;
        dialog.showsHiddenFiles = false;
        dialog.allowsMultipleSelection = false;
        dialog.canChooseDirectories = false;
        
        if (dialog.runModal() == NSApplication.ModalResponse.OK) {
            let result = dialog.url // Pathname of the file
            if (result != nil) {
                return result!.path
            } else {
                let alert = NSAlert()
                alert.messageText = "The file is not vaild!"
                alert.runModal()
                return nil
            }
        }
        // User clicked on "Cancel"
        return nil
    }
    
    func numberOfItems(in comboBox: NSComboBox) -> Int {
        return zLvDimLen
    }
    
    func comboBox(_ comboBox: NSComboBox, objectValueForItemAt index: Int) -> Any? {
        return index
    }
}

