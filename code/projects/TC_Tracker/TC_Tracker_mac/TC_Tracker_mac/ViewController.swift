//
//  ViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa
import UniformTypeIdentifiers

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
    @IBOutlet var gridResTextField: NSTextField!
    @IBOutlet var interpSwitch: NSSwitch!
    @IBOutlet var exportBtn: NSButton!
    
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
    @objc private dynamic var shouldInterp = false
    @objc private dynamic var shouldNotInterp: Bool {
        return !shouldInterp
    }
    @objc private dynamic var interpLabelStr: String {
        shouldInterp ? "插值到格点分辨率：" : "不插值"
    }
    
    @objc private dynamic var isWrfoutFile = false
    private var zLvDimLen: Int = 0
    /// 给getZLvDimLenName方法使用
    private var theVarStr: String {
        vorVarStr.isEmpty ? uwndVarStr : vorVarStr
    }
//    private var realTCs: [Typhoon] = []
    private var tcs: TCs?
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        isWrfoutIcon.imageScaling = .scaleAxesIndependently
        
        setGridResFormat()
        
        zLvComboBox.dataSource = self
        NotificationCenter.default.addObserver(self, selector: #selector(getVarNames(_:)), name: NSNotification.Name(rawValue: "AllVarNamesGet"), object: nil)
        
        interpSwitch.isEnabled = false
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    @IBAction func browseBtnClicked(_ sender: NSButton) {
        let filePath = showFileBrowser()
        
        guard let filePath = filePath else { return }
        
        zLvComboBox.isEnabled = false
        showWebBtn.isEnabled = false
        startTrackingBtn.isEnabled = false

        guard checkFileValid(filePath: filePath) else {
            interpSwitch.isEnabled = false
            return
        }
        
        interpSwitch.isEnabled = true
        zVarStr = ""
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
        if !checkZLvComboBox() { return }
        let (checkPassed, gridResValue) = checkGridResValue()
        if !checkPassed { return }
        selVarNameBtn.isEnabled = false
        startTrackingBtn.isEnabled = false
        zLvComboBox.isEnabled = false
        
        let tracker = NCFileInfo_Wrapper(ncFilePath: filePathTextField.stringValue, isWrfoutFile, timeVarStr, latVarStr, lonVarStr, vorVarStr, uwndVarStr, vwndVarStr, !vorVarStr.isEmpty, (zLvDimLen == 0) ? -1 : zLvComboBox.intValue, gridResValue, "/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/out/")!
        
        guard let progressVC = storyboard?.instantiateController(withIdentifier: "ProgressVC") as? ProgressViewController else { return }
        progressVC.tracker = tracker
        presentAsSheet(progressVC)
        
    }
    
    @IBAction func showWebBtnClicked(_ sender: NSButton) {
        guard let resultVC = storyboard?.instantiateController(withIdentifier: "ResultView") as? ResultViewController else { return }
        guard let tcs = tcs else { return }
//        resultVC.tcsData = realTCs
        resultVC.tcsData = tcs.tcs.compactMap { $0 as? Typhoon }
        presentAsModalWindow(resultVC)
    }
    
    @IBAction func exportBtnClicked(_ sender: NSButton) {
        let savePanel = NSSavePanel()
        savePanel.allowedContentTypes = [.json, UTType(filenameExtension: "pb")!, UTType(filenameExtension: "nc")!]
        savePanel.allowsOtherFileTypes = false
        savePanel.message = "export as json, protobuf, or nc file"
//        savePanel.prompt = "aaa"
//        savePanel.nameFieldLabel = "dddd"
        if (savePanel.runModal() == NSApplication.ModalResponse.OK) {
//            savePanel.typ
            
            guard let filePath = savePanel.url?.path else {
                let alert = NSAlert()
                alert.messageText = "The file path is not valid!"
                alert.runModal()
                return
            }
            let fileExtension = (filePath as NSString).pathExtension
            print(fileExtension)
            
            switch fileExtension {
            case "json":
                NCFileInfo_Wrapper().exportFile_json(tcs, oNcFilePath: filePath)
            case "pb":
                NCFileInfo_Wrapper().exportFile_proto3(tcs, oNcFilePath: filePath)
            case "nc":
                let alert = NSAlert()
                alert.messageText = "Save as compact form?"
                alert.addButton(withTitle: "Not Compact")
                alert.addButton(withTitle: "Compact")
                alert.addButton(withTitle: "Cancel")
                let modalResult = alert.runModal()
                
                switch modalResult {
                case .alertFirstButtonReturn:
                    NCFileInfo_Wrapper().exportFile_nc(tcs, oNcFilePath: filePath, fullCommand: "exportFile_nc")
                case .alertSecondButtonReturn:
                    NCFileInfo_Wrapper().exportFile_nc_compact(tcs, oNcFilePath: filePath, fullCommand: "exportFile_nc_compact")
                default:
                    break
                }
            default:
                let alert = NSAlert()
                alert.messageText = "Extension \"\(fileExtension)\" is not supported"
                alert.informativeText = "\".json\", \".pb\", \".nc\" are supported extensions."
                alert.runModal()
                return
            }
//            NCFileInfo_Wrapper().exportFile_nc(tcs, oNcFilePath: savePanel.url?.path, fullCommand: "exportFile_nc");
        }
    }
    
    
    public func setTCs(tcs: TCs) {
//        self.realTCs = tcs
        self.tcs = tcs
    }
    
    private func checkZLvComboBox() -> Bool {
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
    
    private func checkGridResValue() -> (Bool, Double) {
        if interpSwitch.state == .off {
            return (true, 0)
        }
        let gridResStr = gridResTextField.stringValue
        if gridResStr.isEmpty {
            let alert = NSAlert()
            alert.messageText = "\(gridResStr): 格点分辨率不能为空。"
            alert.runModal()
            return (false, 0)
        }
        if Double(gridResStr) == nil {
            let alert = NSAlert()
            alert.messageText = "\(gridResStr): 输入的格点分辨率不合法。"
            alert.runModal()
            return (false, 0)
        }
        return (true, Double(gridResStr)!)
    }
    
    
    @objc private func getVarNames(_ notification: NSNotification) {
        guard let userInfo = notification.userInfo, let varNames = userInfo["varNames"] as? [String] else { return }
        setVarName(time: varNames[0], lat: varNames[1], lon: varNames[2], vor: varNames[3], u: varNames[4], v: varNames[5])
//        print("varNames:", notification.userInfo!["varNames"])
        
        handleZLevelDim()
        
        startTrackingBtn.isEnabled = true
    }
    
    private func setGridResFormat() {
        let onlyDoubleFormatter = OnlyDoubleValueFormatter()
        onlyDoubleFormatter.numberStyle = .decimal
        onlyDoubleFormatter.usesGroupingSeparator = false
        onlyDoubleFormatter.maximumFractionDigits = 2
        gridResTextField.formatter = onlyDoubleFormatter
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
            zLvComboBox.isEnabled = true
        } else {
            isWrfoutIcon.image = NSImage(systemSymbolName: "multiply.square", accessibilityDescription: nil)
            zLvComboBox.deselectItem(at: zLvComboBox.indexOfSelectedItem)
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
        setVarName(time: allStr, lat: allStr, lon: allStr, vor: allStr, u: allStr, v: allStr)
    }
    
    private func checkFileValid(filePath: String) -> Bool {
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
        return fileValid
    }
    
    private func handleZLevelDim() {
        var zLvDimName: NSString?
        zLvDimLen = Int(NCFileInfo_Wrapper(ncFilePath: filePathTextField.stringValue, timeVarStr, latVarStr, lonVarStr, vorVarStr, uwndVarStr, vwndVarStr, !vorVarStr.isEmpty).getZLvDimLenName(&zLvDimName))
        
        zLvComboBox.deselectItem(at: zLvComboBox.indexOfSelectedItem)
        
        if zLvDimLen == 0 {
            zVarStr = "(无)"
            zLvComboBox.isEnabled = false
            return
        }
        zLvComboBox.isEnabled = true
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
    
    override class func keyPathsForValuesAffectingValue(forKey key: String) -> Set<String> {
        switch key {
        case "shouldNotInterp":
            return ["shouldInterp"]
        case "interpLabelStr":
            return ["shouldInterp"]
        default:
            return []
        }
    }
}

class OnlyDoubleValueFormatter: NumberFormatter {

    override func isPartialStringValid(_ partialString: String, newEditingString newString: AutoreleasingUnsafeMutablePointer<NSString?>?, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {

        // Ability to reset your field (otherwise you can't delete the content)
        // You can check if the field is empty later
        if partialString.isEmpty {
            return true
        }

        // Optional: limit input length
        /*
        if partialString.characters.count>3 {
            return false
        }
        */
        let numSubStrs = partialString.split(separator: ".")
        if (numSubStrs.count == 2) {
            if numSubStrs[1].count > 2 {
                NSSound.beep()
                return false
            }
        }

        // Actual check
        guard let theValue = Double(partialString) else {
            NSSound.beep()
            return false
        }
        if theValue < 0 {
            NSSound.beep()
            return false
        }
        
        return true
    }
}

