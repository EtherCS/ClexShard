source /opt/intel/sgxsdk/environment  
make  
cd ./bin  

# 单实例测试
./appteeprovider  
# 两个相互测试，两个命令行分别执行，
./appteeprovider1  
./appteeprovider2