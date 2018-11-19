# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

-keepattributes Signature
-keep public interface com.oslorde.luadroid.*{
   public <methods>;
}
-keep  class com.oslorde.luadroid.DataMap{
   <init>(...);
}
-keep public !synthetic class com.oslorde.luadroid.*{
   public !synthetic <methods>;
}
-keep public class com.oslorde.luadroid.ScriptContext{
   !synthetic !static <methods>;
   private static *** getSingleInterface(...);
   private static *** weightObject(...);
   private static *** findMembers(...);
   private static *** findMockName(...);
}
-dontnote