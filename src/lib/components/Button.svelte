<script lang="ts">
	export let size: string = '';
	export let ariaLabel: string | null = null;
</script>

<button
	class="inline-flex {size} rounded-md border-2 border-green-700 px-2 py-1 text-sm font-bold text-green-700
        hover:bg-green-700 hover:text-gray-50 focus:outline-none focus:ring focus:ring-green-300
        active:border-green-800 active:bg-green-800 active:text-gray-50 dark:border-green-500
        dark:text-green-500 dark:hover:bg-green-500 dark:hover:text-gray-900 dark:focus:ring-green-300
        dark:active:border-green-600 dark:active:bg-green-600 dark:active:text-gray-900"
	aria-label={ariaLabel}
	on:click
>
	<slot />
</button>
