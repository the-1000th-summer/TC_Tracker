//
//  TCsJsonViewController.swift
//  TC_Tracker_mac
//
//  Created on 2022/7/25.
//

import Cocoa

class TCsJsonViewController: NSViewController {

    @IBOutlet var jsonTextView: NSTextView!
    @IBOutlet var copyButton: NSButton!
    @IBOutlet var cpSuccLabel: NSTextField!
    
    @objc private dynamic var jsonTextViewText: String = ""
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        
    }
    
    public func setTextViewText(_ text: String) {
        jsonTextViewText = text
    }
    
    @IBAction func copyBtnClicked(_ sender: NSButton) {
        NSPasteboard.general.clearContents()
        NSPasteboard.general.setString(jsonTextView.string, forType: .string)
        copyButton.isEnabled = false
        cpSuccLabel.isHidden = false

        DispatchQueue.main.asyncAfter(deadline: .now() + 1.0) {
            self.cpSuccLabel.isHidden = true
            self.copyButton.isEnabled = true
        }
    }
    
}
