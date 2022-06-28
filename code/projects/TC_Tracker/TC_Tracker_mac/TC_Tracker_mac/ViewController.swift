//
//  ViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa

class ViewController: NSViewController {

    @IBOutlet var filePathTextField: NSTextField!
    @IBOutlet var vorLabel: NSTextField!
    @IBOutlet var uwndLabel: NSTextField!
    @IBOutlet var vwndLabel: NSTextField!
    
    
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
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        NotificationCenter.default.addObserver(self, selector: #selector(getVarNames(_:)), name: NSNotification.Name(rawValue: "AllVarNamesGet"), object: nil)
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    @IBAction func browseBtnClicked(_ sender: NSButton) {
        let filePath = showFileBrowser()
        guard let filePath = filePath else { return }

        checkFileValid(filePath: filePath)
        
        
    }
    
    @IBAction func varSelBtnClicked(_ sender: NSButton) {
        print("show")
        timeVarStr = "sadfa"
        guard let varSelectVC = storyboard?.instantiateController(withIdentifier: "VarSelectVC") as? VarSelectViewController else { return }
//        varSelectVC?.view.window?.windowController?.showWindow(self)
        varSelectVC.ncFilePath = filePathTextField.stringValue
//        NSApp.runModal(for: varSelectWC.window!)
//        presentAsModalWindow(varSelectVC)
        presentAsSheet(varSelectVC)
    }
    
    @objc private func getVarNames(_ notification: NSNotification) {
        guard let userInfo = notification.userInfo, let varNames = userInfo["varNames"] as? [String] else { return }
        setVarName(time: varNames[0], lat: varNames[1], lon: varNames[2], vor: varNames[3], u: varNames[4], v: varNames[5])
//        print("varNames:", notification.userInfo!["varNames"])
    }
    
    private func checkIfIsWrfoutFile() {
        
    }
    
    private func setVarName(time: String, lat: String, lon: String, vor: String, u: String, v: String) {
        timeVarStr = time
        latVarStr = lat
        lonVarStr = lon
        vorVarStr = vor
        uwndVarStr = u
        vwndVarStr = v
    }
    
    private func checkFileValid(filePath: String) {
        var fileValidInfo: NSString?;
        let fileValid = NCFileInfo_Wrapper(ncFilePath: filePath).checkFileValid(&fileValidInfo);
        
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
    
}

