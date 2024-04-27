<script lang="ts" context="module">
	export type ToastData = {
		title: string;
	};

	const {
		elements: { content, title },
		helpers,
		states: { toasts }
	} = createToaster<ToastData>();

	export const addToast = helpers.addToast;
</script>

<script lang="ts">
	import { createToaster, melt } from '@melt-ui/svelte';
	import { flip } from 'svelte/animate';
	import { cubicInOut } from 'svelte/easing';
	import { fade } from 'svelte/transition';
</script>

<div class="relative z-50 flex h-full flex-col-reverse items-center justify-start gap-3 p-6">
	{#each $toasts as { id, data } (id)}
		<div
			use:melt={$content(id)}
			animate:flip={{ delay: 300, duration: 500, easing: cubicInOut }}
			transition:fade={{ duration: 225, easing: cubicInOut }}
			class="pointer-events-auto rounded-lg border border-gray-300 bg-gray-100 px-2 py-1 font-medium text-gray-800
				dark:border-gray-700 dark:bg-gray-800 dark:text-gray-200"
		>
			<div class="relative whitespace-nowrap p-3">
				<h3 use:melt={$title(id)}>
					{data.title}
				</h3>
			</div>
		</div>
	{/each}
</div>
