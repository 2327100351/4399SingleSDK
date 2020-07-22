# 激活模块API接入文档  


|文档版本           |变更时间       |变更内容       |  
|:----              |:----          |:----         |  
|v1.0.0             |__2020-01-17__ |初始化文档，并加入主要内容|   
|v1.0.2             |__2020-01-19__ |修复文件冲突|   
|v1.1.0             |__2020-03-17__ |替换https协议| 
|v1.1.1             |__2020-05-28__ |支持跳转外部网页| 

## 一、关于 SDK
激活模块，提供游戏激活码的功能，在游戏启动时检查激活，若运营配置了需要激活，则会弹出激活对话框；  
只有用户成功激活后，才能继续游戏，否则会卡在对话框处。

激活模块可单独接入，单机SDK默认集成激活模块

*注意：单独激活模块时，要确认游戏本身是否直接或间接接入了其他激活模块*

## 二、重要概念说明
- Game Key：或`gameKey`, 游戏的唯一ID，生成和验证激活码需要的基础参数，可在[4399开放平台](https://open.4399.cn/main/home)申请；
- 激活码：一串数字或字母的组合，由运营投放在4399游戏盒等帖子内、控制用户是否可以进入游戏的方式
  
## 三、接入流程

### 准备阶段      
- 1、下载激活模块 [v1.1.3 res+jar](http://common:kCcy8iper6@sdkftp.4399doc.com/external/activation/1.1/4399ActivationSDK-v1.1.3+10.zip)
- 2、在[4399开放平台](https://open.4399.cn/main/home)注册游戏，经运营快速审核后，会得到`gameKey`（接过其他SDK的游戏，已经有此参数，可直接使用）  
 
### 工程配置

#### 依赖
 - 合并res文件夹下的资源
 - 添加jar至android工程下libs目录

#### AndroidManifest 配置
```
<!-- 接入方应在调用接口前，申请好运行时权限 -->
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
<uses-permission android:name="android.permission.READ_PHONE_STATE"/>
```
**接入方应在调用接口前，申请好运行时权限，可以使用[EasyPemissions](https://github.com/googlesamples/easypermissions)第三方类库快速处理**
#### 混淆配置
如果需要进行代码混淆，请不要混淆jar包下的类
```
-keep public class cn.m4399.activation.api.** {*;}
```

### 代码调用


#### 激活接口
在游戏界面Activity创建完成后调用如下代码

```java
public class MainActivity extends Activity
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        ....
        
        GameActivation.activate(activity, "xxxx", false);
    }
    
}
```
