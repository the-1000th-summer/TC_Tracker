//
//  VarSelectViewController.swift
//  TC_Tracker_mac
//
//  Created on 2022/6/25.
//

import Cocoa

class VarSelectViewController: NSViewController, NSComboBoxDataSource {

    @IBOutlet var vorRadioBtn: NSButton!
    
    @IBOutlet var vorNameComboBox: NSComboBox!
    @IBOutlet var uwndNameComboBox: NSComboBox!
    @IBOutlet var vwndNameComboBox: NSComboBox!
    
    @IBOutlet var uwndLabel: NSTextField!
    @IBOutlet var vwndLabel: NSTextField!
    
    
    private var vorNameIsEnabled = true

    
    public var ncFilePath: String = ""
    private var varsName: [String] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        vorNameComboBox.dataSource = self
        uwndNameComboBox.dataSource = self
        vwndNameComboBox.dataSource = self
    }
    
    override func viewWillAppear() {
        super.viewWillAppear()
        
        varsName = NCFileInfo_Wrapper(ncFilePath: ncFilePath).getVarsName().compactMap { $0 as? String }
        if varsName.isEmpty {
            let alert = NSAlert()
            alert.messageText = "此文件无变量！请检查文件内容"
            alert.runModal()
            view.window?.close()
        }
    }
    
    @IBAction func radioButtonChanged(_ sender: NSButton) {
//        vorNameIsEnabled = (sender.state == vorRadioBtn.state)
//        windNameIsEnabled = !vorNameIsEnabled
        vorNameIsEnabled = (sender.state == vorRadioBtn.state)
        if vorNameIsEnabled {
            vorNameComboBox.isEnabled = true
            uwndNameComboBox.isEnabled = false
            vwndNameComboBox.isEnabled = false
            
        } else {
            vorNameComboBox.isEnabled = false
            uwndNameComboBox.isEnabled = true
            vwndNameComboBox.isEnabled = true
        }
        uwndLabel.textColor = uwndNameComboBox.isEnabled ? .labelColor : .gray
        vwndLabel.textColor = vwndNameComboBox.isEnabled ? .labelColor : .gray
    }
    
    @IBAction func nextStepBtnClicked(_ sender: NSButton) {
        guard checkComboBoxes() else { return }
        
        guard let secondVC = storyboard?.instantiateController(withIdentifier: "Var2SelectVC") as? Var2SelectViewController else { return }
        secondVC.ncFilePath = ncFilePath
        secondVC.vorOrWindNames = vorNameIsEnabled ? [vorNameComboBox.stringValue] : [uwndNameComboBox.stringValue, vwndNameComboBox.stringValue]
        
        // 切换view controller导致了在secondVC中presentingViewController为nil
        view.window?.contentViewController = secondVC
    }
    
    private func checkComboBoxes() -> Bool {
        let comboBoxes = vorNameIsEnabled ? [vorNameComboBox] : [uwndNameComboBox, vwndNameComboBox]
        
        for comboBox in comboBoxes {
            let selectedIndex = comboBox!.indexOfSelectedItem
            if selectedIndex == -1 {
                let comboBoxValue = comboBox!.stringValue
                let alert = NSAlert()
                alert.messageText = comboBoxValue.isEmpty ? "变量名不能为空！" : "\(comboBoxValue): 该变量名不存在，请重新选择变量名。"
                alert.runModal()
                return false
            }
        }
        return true
    }
    
    
    func numberOfItems(in comboBox: NSComboBox) -> Int {
        return varsName.count
    }
    
    func comboBox(_ comboBox: NSComboBox, objectValueForItemAt index: Int) -> Any? {
        return varsName[index]
    }
    
}
