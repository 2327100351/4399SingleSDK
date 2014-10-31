# 4399运营SDK增量升级说明
## 升级方式

1. 全自动升级。 所有升级处理逻辑及界面显示，都由sdk完成。
2. 自定义界面升级。 游戏方可以拿到升级信息及下载进度，自定义升级界面。

## 全自动升级
当游戏初始化时，SDK将检查后台是否有新版本游戏上线，如果有，则显示更新内容，并提示用户升级。后台在提交新版游戏时自动制作差分包，更新时用户只需下载APK文件中新旧版本有差异的部分。相关更新内容和版本提交事宜，请联系4399相关运营对接人员。全自动更新无需配置和代码接入，SDK在初始化时自动检查更新。

## 自定义升级界面
如果游戏需要自定义更新界面，需告知4399运营人员，由后台关闭全自动更新开关。由游戏方自主接入更新接口。
### 获取版本升级信息
- 接口  

```java
OperateCenter.getInstance().checkUpdateApk
```
- 参数  

| 参数值 | 说明 |
|-------|------|
|Context|Activity的上下文|
|OnCheckFinishedListener|请求结果回调|


`OnCheckFinishedListener`监听器用于返回请求新版本的信息；  

```java
/**
 * 检查新版本后，返回的新版本信息
 * @param checkResultInfo 新版本信息的对象 里面包括服务端返回的升级信息
 * @param newApkFilePath  如果新版本已经下载完毕，则同时返回新版本文件路径； 否则 返回null
 */
 public void onCheckResponse(ApkCheckResult checkResultInfo, String newApkFilePath);
```

`ApkCheckResult`是一个新版本检查结果，主要公有方法：
-`getCode` 网络请求返回码
-`getNewApkInfo` 新版本信息，是一个`GameUpgradeInfo`对象

## 1.2 显示版本升级信息
1. 判断`onCheckResponse`回调返回的`checkResultInfo.getCode()`是否为成功；
2. 如果成功，`checkResultInfo.getNewApkInfo()`获取新版本信息，用于显示升级弹窗界面；

其中公有方法：  

| 方法名 | 说明 |
|-------|------|
|getUpdateMsg    |获取升级的log信息|
|getVersion      |获取版本名字|
|getPatchSize    |获取补丁大小|
|getGameSize     |获取游戏包的大小|
|checkHavePatch  |判断是否有增量包 true代表补丁包； false代表全包下载|
|isCompel        |判断是否强制升级|
|getDate         |获取新版本发布日期|


## 1.3 启动升级
### 接口
```java
OperateCenter.getInstance().doUpdate
```
### 传入参数
| 参数值 | 说明 |
|-------|------|
|Context|Activity的上下文|
|OnUpdateListener|升级过程的监听器，反馈进度和升级结果|


附：`OnUpdateListener`监听器的定义如下：  
```java
 public interface OnUpdateListener
    {
	/**
	 *
	 * Description: 更新完成
	 *
	 * @param success 是否成功
	 * @param resultCode 操作返回码
	 */
	public void onUpdateSuccess(File newApkFile);
	public void onUpdateFail(int resultCode);
	
	/**
	 * 检查新版本后，返回的新版本信息
	 * @param checkResultInfo 新版本信息的对象 里面包括服务端返回的升级信息
	 */
	public void onUpdateStart();
	
	/**
	 * 升级过程，下载补丁包或全包的进度；
	 * 如果服务端有生成对应的补丁包，就下载补丁包，sdk帮忙做合成补丁包为新版的apk；
	 * 
	 * @param progress 	当前已下载的大小
	 * @param max	      	文件的总大小
	 */
	public void onUpdateProgress(long progress, long max);
	
    }
```