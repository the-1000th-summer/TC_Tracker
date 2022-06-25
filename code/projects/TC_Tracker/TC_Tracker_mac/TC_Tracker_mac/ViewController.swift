//
//  ViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

import Cocoa

class ViewController: NSViewController {

    @IBOutlet var filePathTextField: NSTextField!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }

    @IBAction func browseBtnClicked(_ sender: NSButton) {
        let filePath = showFileBrowser()
        guard let filePath = filePath else { return }

        filePathTextField.stringValue = filePath
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

