#include <ntddk.h>

#define DEBUG_PRINT(format, ...) DbgPrint("[USB Driver] " format "\n", ##__VA_ARGS__)

// Hàm xử lý IRP_MJ_READ
NTSTATUS HandleReadIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // TODO: Xử lý đọc dữ liệu từ USB

    // Hiển thị thông báo trên Debug View
    DEBUG_PRINT("");
    KdPrint(("Copying data from USB"));
    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý IRP_MJ_WRITE
NTSTATUS HandleWriteIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // TODO: Xử lý ghi dữ liệu vào USB

    // Hiển thị thông báo trên Debug View
    DEBUG_PRINT("");
    KdPrint(("Copying data to USB"));
    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý các IRP khác
NTSTATUS HandleOtherIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    // TODO: Xử lý các loại IRP khác nếu cần thiết

    // Gọi hàm xử lý IRP mặc định
    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(DeviceObject, Irp);
}

// Hàm xử lý IRP_MJ_CREATE và IRP_MJ_CLOSE
NTSTATUS HandleCreateCloseIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // Hiển thị thông báo trên Debug View
    DEBUG_PRINT("");
    KdPrint(("Create/Close IRP"));
    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý IRP
NTSTATUS DispatchIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_READ:
        status = HandleReadIrp(DeviceObject, Irp);
        break;

    case IRP_MJ_WRITE:
        status = HandleWriteIrp(DeviceObject, Irp);
        break;

    case IRP_MJ_CREATE:
    case IRP_MJ_CLOSE:
        status = HandleCreateCloseIrp(DeviceObject, Irp);
        break;

    default:
        status = HandleOtherIrp(DeviceObject, Irp);
        break;
    }

    return status;
}

// Hàm xử lý các IRP tiếp theo
NTSTATUS DispatchNextIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    // Chuyển IRP cho lớp tiếp theo
    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(DeviceObject, Irp);
}

// Hàm xử lý IRP tiếp theo
NTSTATUS DispatchDriver(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    // Xử lý IRP
    NTSTATUS status = DispatchIrp(DeviceObject, Irp);

    // Xử lý IRP tiếp theo nếu không thành công
    if (status != STATUS_SUCCESS)
    {
        status = DispatchNextIrp(DeviceObject, Irp);
    }

    return status;
}

// Hàm xử lý các yêu cầu IOCTL
NTSTATUS DispatchIoctl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // TODO: Xử lý các yêu cầu IOCTL nếu cần thiết

    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý các yêu cầu I/O
NTSTATUS DispatchRequest(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_DEVICE_CONTROL:
        status = DispatchIoctl(DeviceObject, Irp);
        break;

    default:
        status = DispatchDriver(DeviceObject, Irp);
        break;
    }

    return status;
}

// Hàm xử lý IRP_MJ_POWER
NTSTATUS DispatchPower(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // TODO: Xử lý các yêu cầu Power nếu cần thiết

    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    PoStartNextPowerIrp(Irp);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý IRP_MJ_PNP
NTSTATUS DispatchPnP(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (irpStack->MinorFunction)
    {
    case IRP_MN_QUERY_REMOVE_DEVICE:
        // The device is about to be removed
        KdPrint(("USB Storage Device Query Remove\n"));
        break;


    case IRP_MN_REMOVE_DEVICE:
        //IRP_MN_DEVICE_REMOVE_COMPLETE
        // The device has been removed
        KdPrint(("Unplug USB Device\n"));
        break;

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:
        // The device's usage notification state has changed
        KdPrint(("USB Storage Device Usage Notification\n"));
        break;

    case IRP_MN_START_DEVICE:
        // The device has started
        KdPrint(("Plug USB Device\n"));

        // TODO: Implement your device start logic here

        break;

    case IRP_MN_STOP_DEVICE:
        // The device has stopped
        KdPrint(("USB Storage Device Stop\n"));

        // TODO: Implement your device stop logic here

        break;

    default:
        // Pass the IRP down the stack
        status = IoCallDriver(DeviceObject, Irp);
        break;
    }

    // Complete the IRP
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}
VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    // Print a message to indicate the driver is being unloaded
    KdPrint(("USB Storage Driver Unloaded\n"));
}

// Hàm xử lý các IRP
NTSTATUS DriverDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    UCHAR majorFunction = IoGetCurrentIrpStackLocation(Irp)->MajorFunction;

    switch (majorFunction)
    {
    case IRP_MJ_CREATE:
    case IRP_MJ_CLOSE:
    case IRP_MJ_READ:
    case IRP_MJ_WRITE:
        status = DispatchRequest(DeviceObject, Irp);
        break;

    case IRP_MJ_POWER:
        status = DispatchPower(DeviceObject, Irp);
        break;

    case IRP_MJ_PNP:
        
        status = DispatchPnP(DeviceObject, Irp);
        break;

    default:
        status = DispatchDriver(DeviceObject, Irp);
        break;
    }

    return status;
}

// Hàm xử lý IRP_MJ_CREATE
NTSTATUS HandleCreateIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // Hiển thị thông báo trên Debug View
    DEBUG_PRINT("");
    KdPrint(("Create IRP"));
    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý IRP_MJ_CLOSE
NTSTATUS HandleCloseIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // Hiển thị thông báo trên Debug View
    DEBUG_PRINT("");
    KdPrint(("Close IRP"));
    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

// Hàm xử lý IRP_MJ_DEVICE_CONTROL
NTSTATUS HandleDeviceControlIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    // TODO: Xử lý các yêu cầu IOCTL nếu cần thiết

    // Hoàn thành IRP
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    // Đăng ký hàm xử lý IRP
    DriverObject->MajorFunction[IRP_MJ_CREATE] = HandleCreateIrp;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = HandleCloseIrp;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchRequest;
    DriverObject->MajorFunction[IRP_MJ_POWER] = DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_PNP] = DispatchPnP;

    DriverObject->DriverUnload = DriverUnload;

    // Hiển thị thông báo trên Debug View
    DEBUG_PRINT("");
    KdPrint(("USB Driver Loaded"));
    return status;
}
