#include <sys/stat.h>
#include <sys/types.h>

bool write_ca_pem() {
    char *cert = "-----BEGIN CERTIFICATE-----\nMIIDwzCCAqugAwIBAgIBATANBgkqhkiG9w0BAQsFADCBgjELMAkGA1UEBhMCREUx\nKzApBgNVBAoMIlQtU3lzdGVtcyBFbnRlcnByaXNlIFNlcnZpY2VzIEdtYkgxHzAd\nBgNVBAsMFlQtU3lzdGVtcyBUcnVzdCBDZW50ZXIxJTAjBgNVBAMMHFQtVGVsZVNl\nYyBHbG9iYWxSb290IENsYXNzIDIwHhcNMDgxMDAxMTA0MDE0WhcNMzMxMDAxMjM1\nOTU5WjCBgjELMAkGA1UEBhMCREUxKzApBgNVBAoMIlQtU3lzdGVtcyBFbnRlcnBy\naXNlIFNlcnZpY2VzIEdtYkgxHzAdBgNVBAsMFlQtU3lzdGVtcyBUcnVzdCBDZW50\nZXIxJTAjBgNVBAMMHFQtVGVsZVNlYyBHbG9iYWxSb290IENsYXNzIDIwggEiMA0G\nCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCqX9obX+hzkeXaXPSi5kfl82hVYAUd\nAqSzm1nzHoqvNK38DcLZSBnuaY/JIPwhqgcZ7bBcrGXHX+0CfHt8LRvWurmAwhiC\nFoT6ZrAIxlQjgeTNuUk/9k9uN0goOA/FvudocP05l03Sx5iRUKrERLMjfTlH6VJi\n1hKTXrcxlkIF+3anHqP1wvzpesVsqXFP6st4vGCvx9702cu+fjOlbpSD8DT6Iavq\njnKgP6TeMFvvhk1qlVtDRKgQFRzlAVfFmPHmBiiRqiDFt1MmUUOyCxGVWOHAD3bZ\nwI18gfNycJ5v/hqO2V81xrJvNHy+SE/iWjnX2J14np+GPgNeGYtEotXHAgMBAAGj\nQjBAMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgEGMB0GA1UdDgQWBBS/\nWSA2AHmgoCJrjNXyYdK4LMuCSjANBgkqhkiG9w0BAQsFAAOCAQEAMQOiYQsfdOhy\nNsZt+U2e+iKo4YFWz827n+qrkRk4r6p8FU3ztqONpfSO9kSpp+ghla0+AGIWiPAC\nuvxhI+YzmzB6azZie60EI4RYZeLbK4rnJVM3YlNfvNoBYimipidx5joifsFvHZVw\nIEoHNN/q/xWA5brXethbdXwFeilHfkCoMRN3zUA7tFFHei4R40cR3p1m0IvVVGb6\ng1XqfMIpiRvpb7PO4gWEyS8+eIVibslfwXhjdFjASBgMmTnrpMwatXlajRWc2BQN\n9noHV8cigwUtPJslJj0Ys6lDfMjIq2SPDqO/nBudMNva0Bkuqjzx+zOAduTNrRlP\nBSeOE6Fuwg==\n-----END CERTIFICATE-----\n-----BEGIN CERTIFICATE-----\nMIIFEjCCA/qgAwIBAgIJAOML1fivJdmBMA0GCSqGSIb3DQEBCwUAMIGCMQswCQYD\nVQQGEwJERTErMCkGA1UECgwiVC1TeXN0ZW1zIEVudGVycHJpc2UgU2VydmljZXMg\nR21iSDEfMB0GA1UECwwWVC1TeXN0ZW1zIFRydXN0IENlbnRlcjElMCMGA1UEAwwc\nVC1UZWxlU2VjIEdsb2JhbFJvb3QgQ2xhc3MgMjAeFw0xNjAyMjIxMzM4MjJaFw0z\nMTAyMjIyMzU5NTlaMIGVMQswCQYDVQQGEwJERTFFMEMGA1UEChM8VmVyZWluIHp1\nciBGb2VyZGVydW5nIGVpbmVzIERldXRzY2hlbiBGb3JzY2h1bmdzbmV0emVzIGUu\nIFYuMRAwDgYDVQQLEwdERk4tUEtJMS0wKwYDVQQDEyRERk4tVmVyZWluIENlcnRp\nZmljYXRpb24gQXV0aG9yaXR5IDIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\nAoIBAQDLYNf/ZqFBzdL6h5eKc6uZTepnOVqhYIBHFU6MlbLlz87TV0uNzvhWbBVV\ndgfqRv3IA0VjPnDUq1SAsSOcvjcoqQn/BV0YD8SYmTezIPZmeBeHwp0OzEoy5xad\nrg6NKXkHACBU3BVfSpbXeLY008F0tZ3pv8B3Teq9WQfgWi9sPKUA3DW9ZQ2PfzJt\n8lpqS2IB7qw4NFlFNkkF2njKam1bwIFrEczSPKiL+HEayjvigN0WtGd6izbqTpEp\nPbNRXK2oDL6dNOPRDReDdcQ5HrCUCxLx1WmOJfS4PSu/wI7DHjulv1UQqyquF5de\nM87I8/QJB+MChjFGawHFEAwRx1npAgMBAAGjggF0MIIBcDAOBgNVHQ8BAf8EBAMC\nAQYwHQYDVR0OBBYEFJPj2DIm2tXxSqWRSuDqS+KiDM/hMB8GA1UdIwQYMBaAFL9Z\nIDYAeaCgImuM1fJh0rgsy4JKMBIGA1UdEwEB/wQIMAYBAf8CAQIwMwYDVR0gBCww\nKjAPBg0rBgEEAYGtIYIsAQEEMA0GCysGAQQBga0hgiweMAgGBmeBDAECAjBMBgNV\nHR8ERTBDMEGgP6A9hjtodHRwOi8vcGtpMDMzNi50ZWxlc2VjLmRlL3JsL1RlbGVT\nZWNfR2xvYmFsUm9vdF9DbGFzc18yLmNybDCBhgYIKwYBBQUHAQEEejB4MCwGCCsG\nAQUFBzABhiBodHRwOi8vb2NzcDAzMzYudGVsZXNlYy5kZS9vY3NwcjBIBggrBgEF\nBQcwAoY8aHR0cDovL3BraTAzMzYudGVsZXNlYy5kZS9jcnQvVGVsZVNlY19HbG9i\nYWxSb290X0NsYXNzXzIuY2VyMA0GCSqGSIb3DQEBCwUAA4IBAQCHC/8+AptlyFYt\n1juamItxT9q6Kaoh+UYu9bKkD64ROHk4sw50unZdnugYgpZi20wz6N35at8yvSxM\nR2BVf+d0a7Qsg9h5a7a3TVALZge17bOXrerufzDmmf0i4nJNPoRb7vnPmep/11I5\nLqyYAER+aTu/de7QCzsazeX3DyJsR4T2pUeg/dAaNH2t0j13s+70103/w+jlkk9Z\nPpBHEEqwhVjAb3/4ru0IQp4e1N8ULk2PvJ6Uw+ft9hj4PEnnJqinNtgs3iLNi4LY\n2XjiVRKjO4dEthEL1QxSr2mMDwbf0KJTi1eYe8/9ByT0/L3D/UqSApcb8re2z2WK\nGqK1chk5\n-----END CERTIFICATE-----\n-----BEGIN CERTIFICATE-----\nMIIFrDCCBJSgAwIBAgIHG2O60B4sPTANBgkqhkiG9w0BAQsFADCBlTELMAkGA1UE\nBhMCREUxRTBDBgNVBAoTPFZlcmVpbiB6dXIgRm9lcmRlcnVuZyBlaW5lcyBEZXV0\nc2NoZW4gRm9yc2NodW5nc25ldHplcyBlLiBWLjEQMA4GA1UECxMHREZOLVBLSTEt\nMCsGA1UEAxMkREZOLVZlcmVpbiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eSAyMB4X\nDTE2MDUyNDExMzg0MFoXDTMxMDIyMjIzNTk1OVowgY0xCzAJBgNVBAYTAkRFMUUw\nQwYDVQQKDDxWZXJlaW4genVyIEZvZXJkZXJ1bmcgZWluZXMgRGV1dHNjaGVuIEZv\ncnNjaHVuZ3NuZXR6ZXMgZS4gVi4xEDAOBgNVBAsMB0RGTi1QS0kxJTAjBgNVBAMM\nHERGTi1WZXJlaW4gR2xvYmFsIElzc3VpbmcgQ0EwggEiMA0GCSqGSIb3DQEBAQUA\nA4IBDwAwggEKAoIBAQCdO3kcR94fhsvGadcQnjnX2aIw23IcBX8pX0to8a0Z1kzh\naxuxC3+hq+B7i4vYLc5uiDoQ7lflHn8EUTbrunBtY6C+li5A4dGDTGY9HGRp5Zuk\nrXKuaDlRh3nMF9OuL11jcUs5eutCp5eQaQW/kP+kQHC9A+e/nhiIH5+ZiE0OR41I\nX2WZENLZKkntwbktHZ8SyxXTP38eVC86rpNXp354ytVK4hrl7UF9U1/Isyr1ijCs\n7RcFJD+2oAsH/U0amgNSoDac3iSHZeTn+seWcyQUzdDoG2ieGFmudn730Qp4PIdL\nsDfPU8o6OBDzy0dtjGQ9PFpFSrrKgHy48+enTEzNAgMBAAGjggIFMIICATASBgNV\nHRMBAf8ECDAGAQH/AgEBMA4GA1UdDwEB/wQEAwIBBjApBgNVHSAEIjAgMA0GCysG\nAQQBga0hgiweMA8GDSsGAQQBga0hgiwBAQQwHQYDVR0OBBYEFGs6mIv58lOJ2uCt\nsjIeCR/oqjt0MB8GA1UdIwQYMBaAFJPj2DIm2tXxSqWRSuDqS+KiDM/hMIGPBgNV\nHR8EgYcwgYQwQKA+oDyGOmh0dHA6Ly9jZHAxLnBjYS5kZm4uZGUvZ2xvYmFsLXJv\nb3QtZzItY2EvcHViL2NybC9jYWNybC5jcmwwQKA+oDyGOmh0dHA6Ly9jZHAyLnBj\nYS5kZm4uZGUvZ2xvYmFsLXJvb3QtZzItY2EvcHViL2NybC9jYWNybC5jcmwwgd0G\nCCsGAQUFBwEBBIHQMIHNMDMGCCsGAQUFBzABhidodHRwOi8vb2NzcC5wY2EuZGZu\nLmRlL09DU1AtU2VydmVyL09DU1AwSgYIKwYBBQUHMAKGPmh0dHA6Ly9jZHAxLnBj\nYS5kZm4uZGUvZ2xvYmFsLXJvb3QtZzItY2EvcHViL2NhY2VydC9jYWNlcnQuY3J0\nMEoGCCsGAQUFBzAChj5odHRwOi8vY2RwMi5wY2EuZGZuLmRlL2dsb2JhbC1yb290\nLWcyLWNhL3B1Yi9jYWNlcnQvY2FjZXJ0LmNydDANBgkqhkiG9w0BAQsFAAOCAQEA\ngXhFpE6kfw5V8Amxaj54zGg1qRzzlZ4/8/jfazh3iSyNta0+x/KUzaAGrrrMqLGt\nMwi2JIZiNkx4blDw1W5gjU9SMUOXRnXwYuRuZlHBQjFnUOVJ5zkey5/KhkjeCBT/\nFUsrZpugOJ8Azv2n69F/Vy3ITF/cEBGXPpYEAlyEqCk5bJT8EJIGe57u2Ea0G7UD\nDDjZ3LCpP3EGC7IDBzPCjUhjJSU8entXbveKBTjvuKCuL/TbB9VbhBjBqbhLzmyQ\nGoLkuT36d/HSHzMCv1PndvncJiVBby+mG/qkE5D6fH7ZC2Bd7L/KQaBh+xFJKdio\nLXUV2EoY6hbvVTQiGhONBg==\n-----END CERTIFICATE-----\n\n";
    char filename[128];
    char *home = getenv("HOME");
    strcpy(filename, home);
    strcat(filename, "/.certs");
    mkdir(filename, 0777);
    strcat(filename, "/ca.pem");
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        return false;
    }
    fprintf(fp, cert);
    fclose(fp);
    return true;
}
