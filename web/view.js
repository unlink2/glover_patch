const app = new Vue({
    el: '#app',
    data: {
        error_message: '',
        warning_message: '',
        shouldPatchEasy: true,
        next: false,
        SETTINGS,

        controller1ImgLoaded: false,
        controller2ImgLoaded: false,
        controllerMemoryImgLoaded: false
    },
    methods: {
        onController1ImgLoad() {
        },
        onController2ImgLoad() {
        },
        onControllerMemoryImgLoad() {
        }
    }
});
