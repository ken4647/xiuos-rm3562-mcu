Actracer接口：

bool AchieveResourceTag(struct TraceTag* target, struct TraceTag* owner, char* name):
    将owner资源（以tag标识）下属名为name的资源标识（tag）的值赋给传入的target。（调用者自行创建target并负责其生命周期）

void* AchieveResource(struct TraceTag* target):
    若资源给定存储的形式为指针，则根据给入的target标识，返回资源的指针。

bool CreateResourceTag(struct TraceTag* new_tag, struct TraceTag* owner, char* name, tracemeta_ac_type type, void* p_resource):
    在owner资源下，创建一个名为name的资源，并将该资源的标识赋值给new_tag。（调用者自行创建target并负责其生命周期）

bool DeleteResource(struct TraceTag* target, struct TraceTag* owner):
    删除owner资源下属以target标识的资源。
