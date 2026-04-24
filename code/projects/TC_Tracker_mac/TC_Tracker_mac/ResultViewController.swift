//
//  ResultViewController.swift
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/7/3.
//

import Cocoa
import WebKit

class ResultViewController: NSViewController, MyContextMenuDelegate {

    @IBOutlet var webView: MyWebView!
    @IBOutlet var nextPageBtn: NSButton!
    
    @objc private dynamic var selectedPathIndex = -1
    @objc private dynamic var pages: Int = 1
    @objc private dynamic var currentPage: Int = 1 {
        didSet {
            draw(currentPage: currentPage)
        }
    }
    @objc private dynamic var pageLabelString: String {
        "\(currentPage) / \(pages)"
    }
    @objc private dynamic var prevPageBtnIsEnabled: Bool {
        currentPage != 1
    }
    @objc private dynamic var nextPageBtnIsEnabled: Bool {
        currentPage != pages
    }
    
    public var tcsData: [Typhoon] = []
    private var tcsDataForJS: [[[String: Float]]] = []
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
//        nextPageBtn.wantsLayer = true
//        nextPageBtn.layer?.backgroundColor = NSColor.black.cgColor
        
        webView.navigationDelegate = self
        
        if let path = Bundle.main.url(forResource: "path", withExtension: "html"){
            let myURLRequest:URLRequest = URLRequest(url: path)
            webView.load(myURLRequest)
        }
    }
    
    override func viewWillAppear() {
        super.viewWillAppear()
        prepareData()
        
    }
    
    override func viewDidAppear() {
        super.viewDidAppear()
        
        view.window?.styleMask.remove(.resizable)
        
        webView.contextMenuDelegate = self
        
        print("did appert")
//        webView.evaluateJavaScript("changeBackgroundColor('red')") { (result, error) in
//            print(error)
//        }
    }
    
    func exportCurrentPathClicked() {
        let tcIndex = (currentPage - 1) * 10 + selectedPathIndex
        let jsonEncoder = JSONEncoder()
        let jsonData = try! jsonEncoder.encode(Array(tcsDataForJS[tcIndex]))
        let jsonStr = String(data: jsonData, encoding: String.Encoding.utf8)!
        
        guard let tcsJsonVC = storyboard?.instantiateController(withIdentifier: "TCsJsonVC") as? TCsJsonViewController else { return }
        tcsJsonVC.setTextViewText(jsonStr)
        presentAsModalWindow(tcsJsonVC)
    }
    
    func checkIfShouldEnableExp() -> Bool {
        getSelectedPathIndex()
        return selectedPathIndex >= 0
    }
    
    private func getSelectedPathIndex() {
        var waiting = true
        webView.evaluateJavaScript("getTcIndex()") { (result, error) in
            if let error = error {
                self.raiseAlert(msgText: "Javascript Error", infoText: error.localizedDescription)
                waiting = false
                return
            }
            if let result = result as? Int { self.selectedPathIndex = result }
            waiting = false
        }
        
        while waiting {
            RunLoop.current.acceptInput(forMode: RunLoop.Mode.default, before: Date.distantFuture)
        }
    }
    
    @IBAction func prevPageBtnClicked(_ sender: NSButton) {
        currentPage -= 1
        selectedPathIndex = -1
    }
    @IBAction func nextPageBtnClicked(_ sender: NSButton) {
        currentPage += 1
        selectedPathIndex = -1
    }
    
    
    private func prepareData() {
        tcsDataForJS = tcsData.map {
            $0.geoCenters.map {
                ["lat": ($0 as! LatLon).lat, "lon": ($0 as! LatLon).lon]
            }
        }
        pages = tcsDataForJS.count / 10 + 1
    }
    
    private func draw(currentPage: Int) {
        let startIndex = (currentPage - 1) * 10
        let tcNumIn1Page = (currentPage == pages) ? (tcsDataForJS.count - startIndex) : 10
        
        let jsonEncoder = JSONEncoder()
        let jsonData = try! jsonEncoder.encode(Array(tcsDataForJS[startIndex..<(startIndex+tcNumIn1Page)]))
        let json = String(data: jsonData, encoding: String.Encoding.utf8)!
        
        webView.evaluateJavaScript("delPath()") { (result, error) in
            if let error = error {
                self.raiseAlert(msgText: "Javascript Error", infoText: error.localizedDescription)
            }
        }
        webView.evaluateJavaScript("drawPath('\(json)')") { (result, error) in
            if let error = error {
                self.raiseAlert(msgText: "Javascript Error", infoText: error.localizedDescription)
            }
        }
    }
    
    private func raiseAlert(msgText: String, infoText: String) {
        let alert = NSAlert()
        alert.messageText = msgText
        alert.informativeText = infoText
        alert.runModal()
    }
    
    override func keyDown(with event: NSEvent) {
        print(event.characters)
    }
    
    override class func keyPathsForValuesAffectingValue(forKey key: String) -> Set<String> {
        switch key {
        case "pageLabelString":
            return ["currentPage", "pages"]
        case "nextPageBtnIsEnabled":
            return ["currentPage", "pages"]
        case "prevPageBtnIsEnabled":
            return ["currentPage"]
        default:
            return []
        }
    }
    

}


class MyWebView: WKWebView {
    var contextMenuDelegate: MyContextMenuDelegate?
    
    override func willOpenMenu(_ menu: NSMenu, with event: NSEvent) {
        // 需要subclass, 因为在点右键之前menu是不存在的(nil), 无法更改menu的内容
        super.willOpenMenu(menu, with: event)
        
        menu.items.removeFirst()      // remove Reload action
        
        menu.items.append(NSMenuItem(title: "导出当前路径...", action: #selector(exportCurrentPathClicked(_:)), keyEquivalent: ""))
        menu.items.last?.isEnabled = contextMenuDelegate?.checkIfShouldEnableExp() ?? false
        
    }
    
    @objc private func exportCurrentPathClicked(_ sender: AnyObject) {
        contextMenuDelegate?.exportCurrentPathClicked()
    }
}

protocol MyContextMenuDelegate {
    func exportCurrentPathClicked()
    func checkIfShouldEnableExp() -> Bool
}

extension ResultViewController: WKNavigationDelegate {

    func webView(_ webView: WKWebView, didFinish navigation: WKNavigation!) {
        webView.evaluateJavaScript("drawBaseMap()", completionHandler: nil)
        draw(currentPage: 1)
    }
}
